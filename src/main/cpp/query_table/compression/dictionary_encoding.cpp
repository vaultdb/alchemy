#include "dictionary_encoding.h"

using namespace vaultdb;

template<>
DictionaryEncoding<Bit>::DictionaryEncoding(QueryTable<Bit> *dst, const int &dst_col, const QueryTable<Bit> *src,
                                             const int &src_col) : ColumnEncoding<Bit>(dst, dst_col) {
    throw; // can't compress under MPC, too many data-dependent branches!
}

template<>
DictionaryEncoding<bool>::DictionaryEncoding(QueryTable<bool> *dst, const int &dst_col, const QueryTable<bool> *src, const int &src_col) : ColumnEncoding<bool>(dst, dst_col) {

    int dst_size_bytes = this->parent_table_->tuple_cnt_ * this->field_size_bytes_;
    dst->column_data_[dst_col] = vector<int8_t>(dst_size_bytes);
    this->column_data_ = dst->column_data_[dst_col].data();
    memset(this->column_data_, 0, dst_size_bytes);
    assert(src->tuple_cnt_ == dst->tuple_cnt_);
    if(src->tuple_cnt_ == 0) return; // not sending party

    map<Field<bool>, int> setup_dictionary;

  // count # of entries to assign codes to them
    for(int i = 0; i < src->tuple_cnt_; ++i) {
        auto field = src->getField(i, src_col);
        if(setup_dictionary.find(field) == setup_dictionary.end()) {
            setup_dictionary[field] = setup_dictionary.size();
        }
    }

    dictionary_entry_cnt_ = setup_dictionary.size();
    field_size_bits_ = ceil(log2(dictionary_entry_cnt_));
    field_size_bytes_ = (field_size_bits_ / 8) + ((field_size_bits_ % 8) != 0); // round up
    desc_ = this->parent_table_->getSchema().getField(dst_col);

    int entry_cnt = src->tuple_cnt_;
    parent_table_->column_data_[column_idx_].resize(entry_cnt * field_size_bytes_);

    // initialize dictionary
    for(auto &pair : setup_dictionary) {
        vector<int8_t> key(field_size_bytes_);
        memcpy(key.data(), &pair.second, field_size_bytes_); // copy over int as bytes
        dictionary_.insert({key, pair.first});
        reverse_dictionary_.insert({pair.first, key});
    }

    // compress the entries
    auto dst_ptr = this->column_data_;
    for(int i = 0; i < src->tuple_cnt_; ++i) {
        auto field = src->getField(i, src_col);
        auto key = reverse_dictionary_[field];
        memcpy(dst_ptr, key.data(), field_size_bytes_);
        dst_ptr += field_size_bytes_;
    }

}

template<typename B>
DictionaryEncoding<B>::DictionaryEncoding(QueryTable<B> *dst, const int &dst_col, DictionaryEncoding<B> &src) : ColumnEncoding<B>(dst, dst_col) {
    dictionary_ = src.dictionary_;
    reverse_dictionary_ = src.reverse_dictionary_;
    dictionary_entry_cnt_ = src.dictionary_entry_cnt_;
    dictionary_idxs_ = src.dictionary_idxs_;
    desc_ = src.desc_;
    ((CompressedTable<B> *) dst)->column_encodings_[dst_col] = this;
}

template<>
Field<bool> DictionaryEncoding<bool>::getField(const int &row) {
    int8_t *src =  this->getFieldPtr(row);
    vector<int8_t> key(field_size_bytes_);
    memcpy(key.data(), src, field_size_bytes_);
    return dictionary_[key];
}

// under MPC all fields become an int of length dictionary_bits_
template<>
Field<Bit> DictionaryEncoding<Bit>::getField(const int &row) {
    Integer src_int = getInt(row);
    return Field<Bit>(FieldType::SECURE_INT, src_int);
}

// field arrives already compressed
template<>
void DictionaryEncoding<Bit>::setField(const int &row, const Field<Bit> &f) {
    auto dst = this->getFieldPtr(row);
    Integer src = f.getInt();
    memcpy(dst, src.bits.data(), this->field_size_bytes_);
}

// field arrives uncompressed.
// use cloneField for direct memcpy
template<>
void DictionaryEncoding<bool>::setField(const int &row, const Field<bool> &f) {
    auto dst = this->getFieldPtr(row);
    auto key = reverse_dictionary_[f];
    memcpy(dst, key.data(), this->field_size_bytes_);
}

template<>
Field<bool> DictionaryEncoding<bool>::getDecompressedField(const int &row) {
    auto key_ptr = getFieldPtr(row);
    vector<int8_t> key(field_size_bytes_);
    memcpy(key.data(), key_ptr, field_size_bytes_);
    return dictionary_[key];
}

// not really designed to be used this way, but including this for completeness
// iterates over dictionary obliviously and returns the matching entry
template<>
Field<Bit> DictionaryEncoding<Bit>::getDecompressedField(const int &row) {
    auto key_ptr = getFieldPtr(row);
    vector<Bit> key(field_size_bits_);
    memcpy(key.data(), key_ptr, field_size_bytes_);

    // need to initialize for oblivious if
    vector<int8_t> first_key(dictionary_idxs_[0]);
    auto dst = dictionary_[first_key];

    for(auto &pair : dictionary_) {
        auto &dict_key = pair.first;
        auto &dict_value = pair.second;
        Bit match = Bit(true, PUBLIC);
        for(int i = 0; i < field_size_bits_; ++i) {
            match = match & (key[i] == dict_key[i]);
        }
        dst = Field<Bit>::If(match, dict_value, dst);
    }
    return dst;
}



template<>
void DictionaryEncoding<Bit>::secretShare(QueryTable<Bit> *d, const int &dst_col) {
        // just clone what we already have
        auto dst_encoding = new DictionaryEncoding<Bit>(d, dst_col, *this);
        ((CompressedTable<Bit> *) d)->column_encodings_[dst_col] = dst_encoding;
}

template<>
void DictionaryEncoding<bool>::secretShare(QueryTable<Bit> *d, const int &dst_col) {
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
        int i = 0;
        // key does not really  matter as long as we process all entries in the same order
        for (auto pos: dictionary_) {
            Integer key_int(dst_encoding->field_size_bits_, i, PUBLIC);
            vector<int8_t> key(dst_encoding->field_size_bytes_);
            memcpy(key.data(), key_int.bits.data(), dst_encoding->field_size_bytes_);
            auto f = pos.second;
            Field<Bit> value = Field<Bit>::secretShareHelper(f, dst_encoding->desc_, sending_party, true);
            // leave reverse dictionary uninitialized for Bit because < operator is not overloaded
            // we can't access it obliviously anyway.
            dst_encoding->dictionary_[key] =  value;
            dst_encoding->dictionary_idxs_[i] = key;
            ++i;
        }

        // to encode values, first serialize them into a bool array
        bool *bools = new bool[row_cnt * dst_encoding->field_size_bits_];
        // for every this->field_size_bytes_, unpack their dst_encoding->field_size_bits_ MSB into bools
        ColumnEncoding<bool>::serializeForSecretSharing(bools, this->column_data_, row_cnt, dst_encoding->field_size_bits_, this->field_size_bytes_);
        manager->feed((Bit *) dst_encoding->column_data_, sending_party, bools, row_cnt * dst_encoding->field_size_bits_);

    } else { // receiving party
        dst_encoding->dictionary_entry_cnt_ = manager->recvPublic();
        dst_encoding->field_size_bits_ = ceil(log2(dst_encoding->dictionary_entry_cnt_));
        PlainField f(this->parent_table_->getSchema().getField(this->column_idx_).getType());
        // can initialize the keys as monotonically increasing ints
        for (int i = 0; i < dst_encoding->dictionary_entry_cnt_; ++i) {
            Integer key_int(dst_encoding->field_size_bits_, i, PUBLIC);
            vector<int8_t> key(dst_encoding->field_size_bytes_);
            memcpy(key.data(), key_int.bits.data(), dst_encoding->field_size_bytes_);
            Field<Bit> value = Field<Bit>::secretShareHelper(f, dst_encoding->desc_, sending_party, false);
            dst_encoding->dictionary_[key] =  value;
            dst_encoding->dictionary_idxs_[i] = key;
        }

        // accept Bit array
        manager->feed((Bit *) dst_encoding->column_data_, sending_party, nullptr, row_cnt * dst_encoding->field_size_bits_);

    }
}


template<>
void DictionaryEncoding<Bit>::revealInsecure(QueryTable<bool> *dst, const int &dst_col, const int &party) {
    auto dst_encoding = new DictionaryEncoding<bool>(dst, dst_col);
    dst_encoding->dictionary_entry_cnt_ = this->dictionary_entry_cnt_;
    dst_encoding->field_size_bits_ = this->field_size_bits_;
    dst_encoding->field_size_bytes_ = (this->field_size_bits_ / 8) +  ((this->field_size_bits_ % 8) != 0);
    int field_bit_cnt = dst_encoding->field_size_bits_; // shortcuts for readability
    int field_byte_cnt = dst_encoding->field_size_bytes_;

    assert(dictionary_idxs_.size() == dictionary_entry_cnt_);
    auto manager = SystemConfiguration::getInstance().emp_manager_;
    cout << "Dictionary size: " << dictionary_.size() << ", expected: " << dst_encoding->dictionary_entry_cnt_ << endl;
    vector<int8_t> key(field_byte_cnt);
    bool *b = new bool[field_bit_cnt];

    // initialize the dictionary
    // need to synchronize on the order in which we reveal, hence dictionary_idxs_
    for(int i = 0; i < dictionary_idxs_.size(); ++i) {
        auto key_bits = dictionary_idxs_[i];
        auto value = dictionary_[key_bits];
        manager->reveal(b, party, (Bit *) key_bits.data(), field_bit_cnt);
        emp::from_bool<int8_t>(b, key.data(), dst_encoding->field_size_bits_, false);
        Field<bool> revealed = value.reveal(desc_, party);
        cout << DataUtilities::printByteArray(key.data(), field_byte_cnt) << " --> " << revealed << endl;
        dst_encoding->dictionary_[key] = revealed;
        dst_encoding->reverse_dictionary_[revealed] = key;
    }
    delete [] b;

    // reveal the column
    int row_cnt = this->parent_table_->tuple_cnt_;
    int reveal_bit_cnt = row_cnt * field_bit_cnt;
    bool *bools = new bool[reveal_bit_cnt];
    manager->reveal(bools, party, (Bit *) this->column_data_, reveal_bit_cnt);

    for(int i = 0; i < row_cnt; ++i) {
        emp::from_bool(bools + i * dst_encoding->field_size_bits_, key.data(), dst_encoding->field_size_bits_, false);
        Field<bool> f = dst_encoding->dictionary_[key];
        dst_encoding->setField(i, f);
    }

    delete [] bools;

}

// already revealed - just copy it over
template<>
void DictionaryEncoding<bool>::revealInsecure(QueryTable<bool> *dst, const int &dst_col, const int &party) {
    auto dst_encoding = new DictionaryEncoding<bool>(dst, dst_col, *this);
    return;
}




template class vaultdb::DictionaryEncoding<bool>;
template class vaultdb::DictionaryEncoding<Bit>;
