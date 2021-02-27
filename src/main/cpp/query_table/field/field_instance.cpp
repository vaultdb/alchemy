#include "field_instance.h"

using namespace vaultdb;

template<typename T, typename P, typename R, typename B>
void vaultdb::FieldInstance<T, P, R, B>::serialize(int8_t *dst) const {
    P payload = getValue();
    size_t size = static_cast<const T*>(this) / 8; // bits --> bytes
    memcpy(dst, (int8_t *) payload, size);
}

/*template<typename T, typename P, typename R, typename B>
std::shared_ptr<Field> FieldInstance<T, P, R, B>::reveal() const  {
    T impl = static_cast<T const &>(*this);
    R revealed = impl.decrypt();
    FieldType type = impl.decryptType();

    return FieldFactory<P>::getField(revealed, type);
}*/
