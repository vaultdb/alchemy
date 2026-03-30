#include <query_table/oram/oram_bucket.h>
#include <query_table/field/field_factory.h>

using namespace vaultdb;


template<typename B>
B OramBucket<B>::isFull(const vector<OramBlock<B>> & blocks) {
    B full(true);

    for(OramBlock<B> b : blocks) {
            full = full & (!b.isDummy());
    }

    return full;
}


template<typename B>
B OramBucket<B>::isEmpty(const vector<OramBlock<B>> & blocks) {
    B empty(true);

    for(OramBlock<B> b : blocks) {
        empty = empty & b.isDummy();
    }

    return empty;

}


template<typename B>
OramBlock<B> OramBucket<B>::readAndRemove(vector<OramBlock<B>> & blocks, const Field<B> &id) {

    OramBlock<B> result = dummy_block_;
    for(int i = 0; i < blocks.size(); ++i) {
        B eq = ((id == blocks[i].id_) & !blocks[i].tuple_.getDummyTag()) ;
        result = OramBlock<B>::If(eq, blocks[i], result);
        B new_dummy = Utilities::If(eq, true, blocks[i].isDummy());
        blocks[i].setDummyTag(new_dummy);
    }

    return result;
}


template<typename B>
OramBlock<B> OramBucket<B>::conditionalReadAndRemove(vector<OramBlock<B>> & blocks, const Field<B> &id, const B &cond) {
    OramBlock<B> result = dummy_block_;
    for(int i = 0; i < blocks.size(); ++i) {
        B eq = ((id == blocks[i].id_) & (!blocks[i].tuple_.getDummyTag())  & cond);
        result = OramBlock<B>::If(eq, blocks[i], result);
        B new_dummy = Utilities::If(eq, true, blocks[i].isDummy());
        blocks[i].setDummyTag(new_dummy);
    }

    return result;
}


template<typename B>
void OramBucket<B>::add(vector<OramBlock<B>> & blocks, const OramBlock<B> &new_block, const Field<B> &id) {
    B added(false);
    for(int i = 0; i < blocks.size(); ++i) {
        B match = ((!blocks[i].isDummy()) & (id == blocks[i].id_));
        added = added | match;
    }

    for(int i = 0; i < blocks.size(); ++i) {
        B should_add = ((!blocks[i].isDummy()) & !added);
        blocks[i] = OramBlock<B>::If(should_add, new_block, blocks[i]);
    }

}


template<typename B>
void OramBucket<B>::conditionalAdd(vector<OramBlock<B>> & blocks, const OramBlock<B> &new_block, const Field<B> &id, const B &cond) {
    B added(cond);
    for(int i = 0; i < blocks.size(); ++i) {
        B match = ((!blocks[i].isDummy()) & (id == blocks[i].id_));
        added = added | match;
    }

    for(int i = 0; i < blocks.size(); ++i) {
        B should_add = ((!blocks[i].isDummy()) & !added);
        blocks[i] = OramBlock<B>::If(should_add, new_block, blocks[i]);
    }

}

template<typename B>
OramBlock<B> OramBucket<B>::pop(vector<OramBlock<B>> & blocks) {
    OramBlock<B> result = dummy_block_;
    B popped(false);
    for(int i = 0; i < blocks.size(); ++i) {
        B should_pop = ((!popped) & (!blocks[i].isDummy()));
        popped = should_pop | popped;
        result = OramBlock<B>::If(should_pop, blocks[i], result);
        B new_dummy = Utilities::If(should_pop, true, blocks[i].isDummy());
        blocks[i].setDummyTag(new_dummy);
    }

    return result;

}


template<typename B>
OramBlock<B> OramBucket<B>::conditionalPop(vector<OramBlock<B>> & blocks, const B &cond) {
    OramBlock<B> result = dummy_block_;
    B popped(cond);
    for(int i = 0; i < blocks.size(); ++i) {
        B should_pop = ((!popped) & (!blocks[i].isDummy()));
        popped = should_pop | popped;
        result = OramBlock<B>::If(should_pop, blocks[i], result);
        B new_dummy = Utilities::If(should_pop, true, blocks[i].isDummy());
        blocks[i].setDummyTag(new_dummy);
    }

    return result;
}




template<typename B>
vector<OramBlock<B>> OramBucket<B>::Xor(const vector<OramBlock<B>> &  a, const vector<OramBlock<B>>  & b) {
    assert(a.size() == b.size());
    assert(*(a[0].tuple_.getSchema()) == *(b[0].tuple_.getSchema()) );
    vector<OramBlock<B>> res(a.size(), OramBlock<B>(a[0].tuple_.getSchema()));

    for(int i = 0; i < res.size(); ++i) {
        res[i] = OramBlock<B>::Xor(a[i], b[i]);
    }

    return res;
}

template<typename B>
Field<B> OramBucket<B>::deepestLevelLocal(const Field<B> &pos, const  Field<B> &path) {
    Field<B> xored = pos ^ path;
   return xored.leadingZeros();

}

// path is INT or SECURE_INT
template<typename B>
vector<Field<B>> OramBucket<B>::deepestBlockShort(const vector<OramBlock<B>> & bucket, const Field<B> & path) {
    vector<Field<B>> deepest(bucket.size());
    for (int j = 0; j < bucket.size(); ++j) {
        deepest[j] = deepestLevelLocal(bucket[j].pos_, path);
    }
    Field<B> max_id = bucket[0].id_;
    Field<B> max_depth = deepest[0];
    B is_dummy = bucket[0].isDummy();
    for (int j = 1; j < bucket.size(); ++j) {
        B greater = deepest[j] >= max_depth;
        greater = (greater & !(bucket[j].isDummy()));
        max_id = Field<B>::If(greater, bucket[j].id_, max_id);
        max_depth = Field<B>::If(greater, deepest[j], max_depth);
        is_dummy = Utilities::If(greater, bucket[j].isDummy(), is_dummy);
    }
     vector<Field<B>> result;
    result.emplace_back(max_id);
    result.emplace_back(max_depth);
    result.emplace_back(Field<B>(TypeUtilities::getFieldType<B>(), is_dummy, 0));

    return result;


}

template<typename B>
void OramBucket<B>::flush(vector<vector<OramBlock<B>>> & blocks,  vector<bool> & path, vector<OramBlock<B>> & srcQueue) {
    // TODO: NYI
}


template<typename B>
vector<emp::Bit> OramBucket<B>::leadingZeros(const vector<emp::Bit> &src) {
    emp::Integer i(src);
    return i.leading_zeros().bits;
}

// TODO: implement this - using EMP code as placeholder
template<typename B>
vector<bool> OramBucket<B>::leadingZeros(const vector<bool> &src) {
    emp::Integer res(src.size(), 0, PUBLIC);
    for(int i = 0; i < src.size(); ++i)
        res.bits[i] = src[i];

    Integer j = res.leading_zeros();

    vector<bool> dst(src.size());

    for(int i = 0; i < src.size(); ++i)
        dst[i] = j[i].reveal();

    return dst;
}



template class vaultdb::OramBucket<bool>;
template class vaultdb::OramBucket<emp::Bit>;

