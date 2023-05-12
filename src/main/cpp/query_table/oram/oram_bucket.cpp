#include <query_table/oram/oram_bucket.h>
#include <query_table/field/field_factory.h>

using namespace vaultdb;

template<typename B>
OramBucket<B>::OramBucket(const shared_ptr <QuerySchema> &schema, int z, int my_depth) :  bucket_depth_(my_depth), dummy_block_(schema) {
    blocks_ = vector<OramBlock<B>>(z);
    for(int i = 0; i < z; ++i)
        blocks_[i] = OramBucket<B>(schema);

}

template<typename B>
B OramBucket<B>::isFull() {
    B full(true);

    for(OramBlock<B> b : blocks_) {
            full = full & (!b.isDummy());
    }

    return full;
}


template<typename B>
B OramBucket<B>::isEmpty() {
    B empty(true);

    for(OramBlock<B> b : blocks_) {
        empty = empty & b.isDummy();
    }

    return empty;

}


template<typename B>
OramBlock<B> OramBucket<B>::readAndRemove(const Field<B> &id) {

    OramBlock<B> result = dummy_block_;
    for(int i = 0; i < blocks_.size(); ++i) {
        B eq = ((id == blocks_[i].id_) & !blocks_[i].tuple_.getDummyTag()) ;
        result = OramBlock<B>::If(eq, blocks_[i], result);
        B new_dummy = emp::If(eq, true, blocks_[i].isDummy());
        blocks_[i].setDummyTag(new_dummy);
    }

    return result;
}


template<typename B>
OramBlock<B> OramBucket<B>::conditionalReadAndRemove(const Field<B> &id, const B &cond) {
    OramBlock<B> result = dummy_block_;
    for(int i = 0; i < blocks_.size(); ++i) {
        B eq = ((id == blocks_[i].id_) & (!blocks_[i].tuple_.getDummyTag())  & cond);
        result = OramBlock<B>::If(eq, blocks_[i], result);
        B new_dummy = emp::If(eq, true, blocks_[i].isDummy());
        blocks_[i].setDummyTag(new_dummy);
    }

    return result;
}


template<typename B>
void OramBucket<B>::add(const OramBlock<B> &block, const Field<B> &id) {

}


template<typename B>
void OramBucket<B>::conditionalAdd(const OramBlock<B> &block, const Field<B> &id, const B &cond) {

}

template<typename B>
OramBlock<B> OramBucket<B>::pop() {
    return OramBlock<B>(std::shared_ptr());
}


template<typename B>
OramBlock<B> OramBucket<B>::conditionalPop(const B &cond) {
    return OramBlock<B>(std::shared_ptr());
}

template<typename B>
OramBlock<B> OramBucket<B>::mux(const OramBlock<B> &a, const OramBlock<B> &b, B cond) {
    return OramBlock<B>(std::shared_ptr());
}



template<typename B>
OramBlock<B> OramBucket<B>::Xor(const OramBucket<B> & a, const OramBucket<B> & b) {

}