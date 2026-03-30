#include "plain_dictionary_encoding.h"
#include "secure_dictionary_encoding.h"


using namespace vaultdb;

void DictionaryEncoding<bool>::secretShare(QueryTable<Bit> *d, const int &dst_col)  {

     // no bit packing, just taking a plaintext column and converting it into a secret shared one
            // assume source compression scheme is same as dst compression scheme
            assert(d->storageModel() == StorageModel::COMPRESSED_STORE);
            assert(!this->parent_table_->isEncrypted());

            CompressedTable<Bit> *dst = (CompressedTable<Bit> *) d;

            SystemConfiguration &s = SystemConfiguration::getInstance();
            auto manager = s.emp_manager_;
            int sending_party = manager->sendingParty();
            assert(sending_party !=  0); // only want to compress data from one party, not split over 2 or more secret sharing hosts

            bool sender = (s.party_ == sending_party);

            int row_cnt = dst->tuple_cnt_;
            auto dst_encoding = new DictionaryEncoding<Bit>(dst, dst_col);
            dst_encoding->desc_ = dst->getSchema().getField(dst_col);

            if (sender) {
                manager->sendPublic(dictionary_entry_cnt_);
                dst_encoding->dictionary_entry_cnt_ = dictionary_entry_cnt_;
                dst_encoding->field_size_bits_ = field_size_bits_;
                dst_encoding->field_size_bytes_ = dst_encoding->field_size_bits_ * sizeof(Bit);
                assert(dictionary_.size() == dictionary_entry_cnt_);

                for(int i = 0; i < dictionary_entry_cnt_; ++i) {
                    Integer key_int(dst_encoding->field_size_bits_, i, PUBLIC);
                    auto f = dictionary_[i];
                    auto value = f.secret_share();
                    auto pair = std::pair<Integer, Field<Bit> >(key_int, value);
                    dst_encoding->dictionary_.push_back(pair);
                }

                // to encode values, first serialize them into a bool array
                bool *bools = new bool[row_cnt * dst_encoding->field_size_bits_];
                // for every this->field_size_bytes_, unpack their dst_encoding->field_size_bits_ LSB into bools
                ColumnEncoding<bool>::serializeForSecretSharing(bools, this->column_data_, row_cnt, dst_encoding->field_size_bits_, this->field_size_bytes_);
                manager->feed((Bit *) dst_encoding->column_data_, sending_party, bools, row_cnt * dst_encoding->field_size_bits_);
            } else { // receiving party
                dst_encoding->dictionary_entry_cnt_ = manager->recvPublic();
                dst_encoding->field_size_bits_ = ceil(log2(dst_encoding->dictionary_entry_cnt_));
                PlainField f(this->parent_table_->getSchema().getField(this->column_idx_).getType());
                //  initialize the keys as monotonically increasing ints
                for (int i = 0; i < dst_encoding->dictionary_entry_cnt_; ++i) {
                    Integer key_int(dst_encoding->field_size_bits_, i, PUBLIC);
                    Field<Bit> value = Field<Bit>::secretShareHelper(f, dst_encoding->desc_, sending_party, false);
                    auto pair = std::pair<Integer, Field<Bit> >(key_int, value);
                    dst_encoding->dictionary_.push_back(pair);
                }

                // accept Bit array
                manager->feed((Bit *) dst_encoding->column_data_, sending_party, nullptr, row_cnt * dst_encoding->field_size_bits_);

            }
        }