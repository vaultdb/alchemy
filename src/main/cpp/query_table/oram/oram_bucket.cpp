#include <query_table/oram/oram_bucket.h>
#include <query_table/field/field_factory.h>

using namespace vaultdb;

template<typename B>
OramBucket<B>::OramBucket(const shared_ptr <QuerySchema> &schema, int z) :  dummy_block_(schema) {
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
void OramBucket<B>::add(const OramBlock<B> &new_block, const Field<B> &id) {
    B added(false);
    for(int i = 0; i < blocks_.size(); ++i) {
        B match = ((!blocks_[i].isDummy()) & (id == blocks_[i].id_));
        added |= match;
    }

    for(int i = 0; i < blocks_.size(); ++i) {
        B should_add = ((!blocks_[i].isDummy()) & !added);
        blocks_[i] = OramBlock<B>::If(should_add, new_block, blocks_[i]);
    }

}


template<typename B>
void OramBucket<B>::conditionalAdd(const OramBlock<B> &new_block, const Field<B> &id, const B &cond) {
    B added(cond);
    for(int i = 0; i < blocks_.size(); ++i) {
        B match = ((!blocks_[i].isDummy()) & (id == blocks_[i].id_));
        added |= match;
    }

    for(int i = 0; i < blocks_.size(); ++i) {
        B should_add = ((!blocks_[i].isDummy()) & !added);
        blocks_[i] = OramBlock<B>::If(should_add, new_block, blocks_[i]);
    }

}

template<typename B>
OramBlock<B> OramBucket<B>::pop() {
    OramBlock<B> result = dummy_block_;
    B popped(false);
    for(int i = 0; i < blocks_.size(); ++i) {
        B should_pop = ((!popped) & (!blocks_[i].isDummy()));
        popped |= should_pop;
        result = OramBlock<B>::If(should_pop, blocks_[i], result);
        B new_dummy = emp::If(should_pop, true, blocks_[i].isDummy());
        blocks_[i].setDummyTag(new_dummy);
    }

    return result;

}


template<typename B>
OramBlock<B> OramBucket<B>::conditionalPop(const B &cond) {
    OramBlock<B> result = dummy_block_;
    B popped(cond);
    for(int i = 0; i < blocks_.size(); ++i) {
        B should_pop = ((!popped) & (!blocks_[i].isDummy()));
        popped |= should_pop;
        result = OramBlock<B>::If(should_pop, blocks_[i], result);
        B new_dummy = emp::If(should_pop, true, blocks_[i].isDummy());
        blocks_[i].setDummyTag(new_dummy);
    }

    return result;
}




template<typename B>
OramBlock<B> OramBucket<B>::Xor(const OramBucket<B> & a, const OramBucket<B> & b) {
    assert(a.blocks_.size() == b.blocks_.size());
    assert(*(a.blocks_[0].tuple_.getSchema()) == *(b.blocks_[0].tuple_.getSchema()) );
    OramBucket res(a.blocks_[0].tuple_.getSchema(), a.blocks_.size());

    for(int i = 0; i < res.blocks_.size(); ++i) {
        res.blocks_[i] = OramBlock<B>::Xor(a.blocks_[i], b.blocks_[i];
    }

    return res;
}