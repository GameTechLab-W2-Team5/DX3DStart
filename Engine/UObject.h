#pragma once
#include "UEngineStatics.h"
#include "TArray.h"
#include "ISerializable.h"
#include <type_traits>

// Forward declaration으로 순환 종속성 해결
class UClass;

typedef int int32;
typedef unsigned int uint32;

#define DECLARE_ROOT_UCLASS(ClassName) \
public: \
    virtual UClass* GetClass() const; \
    static UClass* StaticClass(); \
    static TUniquePtr<ClassName> CreateUniqueInstance(); \
private: \
    static UClass* s_StaticClass; \
    static UObject* CreateInstance();

#define IMPLEMENT_ROOT_UCLASS(ClassName) \
UObject* ClassName::CreateInstance() { return new ClassName(); } \
TUniquePtr<ClassName> ClassName::CreateUniqueInstance() { return MakeUnique<ClassName>(); } \
UClass* ClassName::s_StaticClass = UClass::RegisterToFactory( \
    #ClassName, &ClassName::CreateInstance, ""); \
UClass* ClassName::StaticClass() { return s_StaticClass; } \
UClass* ClassName::GetClass() const { return StaticClass(); }

#define DECLARE_UCLASS(ClassName, ParentClass) \
public: \
    using Super = ParentClass; \
    UClass* GetClass() const override; \
    static UClass* StaticClass(); \
    static TUniquePtr<ClassName> CreateUniqueInstance(); \
private: \
    static UClass* s_StaticClass; \
    static UObject* CreateInstance();

#define IMPLEMENT_UCLASS(ClassName, ParentClass) \
UObject* ClassName::CreateInstance() { return new ClassName(); } \
TUniquePtr<ClassName> ClassName::CreateUniqueInstance() { return MakeUnique<ClassName>(); } \
UClass* ClassName::s_StaticClass = UClass::RegisterToFactory( \
    #ClassName, &ClassName::CreateInstance, #ParentClass); \
UClass* ClassName::StaticClass() { return s_StaticClass; } \
UClass* ClassName::GetClass() const { return StaticClass(); }

#define UCLASS_META(ClassName, Key, Value) \
struct _MetaRegister_##ClassName##_##Key { \
    _MetaRegister_##ClassName##_##Key() { ClassName::StaticClass()->SetMeta(#Key, Value); } \
} _MetaRegisterInstance_##ClassName##_##Key;

class UObject : public ISerializable
{
    DECLARE_ROOT_UCLASS(UObject)
private:
    static inline TArray<uint32> FreeIndices;
    static inline uint32 NextFreshIndex = 0;
public:
    static inline TArray<UObject*> GUObjectArray;
    uint32 UUID;
    uint32 InternalIndex;

    static void CleanupGlobalObjectArray()
    {
        char buffer[256];
        sprintf_s(buffer, "CleanupGlobalObjectArray called: %zu objects in array\n", GUObjectArray.size());
        OutputDebugStringA(buffer);
        
        // 남아있는 객체들의 정보 출력
        size_t remainingCount = 0;
        for (size_t i = 0; i < GUObjectArray.size(); ++i) {
            if (GUObjectArray[i] != nullptr) {
                sprintf_s(buffer, "Remaining object at index %zu, UUID: %u\n", i, GUObjectArray[i]->UUID);
                OutputDebugStringA(buffer);
                remainingCount++;
            }
        }
        sprintf_s(buffer, "Total remaining objects: %zu\n", remainingCount);
        OutputDebugStringA(buffer);
        
        GUObjectArray.clear();
        FreeIndices.clear();
        NextFreshIndex = 0;
        OutputDebugStringA("CleanupGlobalObjectArray completed\n");
    }

    UObject()
    {
        UUID = UEngineStatics::GenUUID();

        // 방법 1: 빠른 추가 - 재사용 인덱스 우선 사용
        if (!FreeIndices.empty())
        {
            // 삭제된 슬롯 재사용 (O(1))
            InternalIndex = FreeIndices.back();
            FreeIndices.pop_back();
            GUObjectArray[InternalIndex] = this;
        }
        else
        {
            // 새 슬롯 할당 (O(1) amortized)
            InternalIndex = NextFreshIndex++;
            GUObjectArray.push_back(this);
        }
    }

    virtual ~UObject()
    {
        // 방법 1: 빠른 삭제 - nullptr 마킹 + 인덱스 재사용 큐에 추가
        if (InternalIndex < GUObjectArray.size() && GUObjectArray[InternalIndex] == this)
        {
            GUObjectArray[InternalIndex] = nullptr;  // O(1)
            FreeIndices.push_back(InternalIndex);    // O(1)
        }
    }

    virtual bool CountOnInspector() {
        return false;
    }

    template<typename T>
    bool IsA() const {
        return GetClass()->IsChildOrSelfOf(T::StaticClass());
    }

    template<typename T>
    T* Cast() {
        return IsA<T>() ? static_cast<T*>(this) : nullptr;
    }

    template<typename T>
    const T* Cast() const {
        return IsA<T>() ? static_cast<const T*>(this) : nullptr;
    }
    // Override new/delete for tracking
    void* operator new(size_t size)
    {
        UEngineStatics::AddAllocation(size);
        return ::operator new(size);
    }

    void operator delete(void* ptr, size_t size)
    {
        if (ptr != nullptr) {
            UEngineStatics::RemoveAllocation(size);
            ::operator delete(ptr);
        }
    }

    // 배치 new/delete도 오버라이드 (필요한 경우)
    void* operator new(size_t size, void* ptr)
    {
        return ptr;  // placement new는 메모리 추적 안함
    }

    void operator delete(void* ptr, void* place)
    {
        // placement delete는 아무것도 안함
    }

    json::JSON Serialize() const override
    {
        return json::JSON();
    }

    bool Deserialize(const json::JSON& data) override
    {
        return true;
    }

    void SetUUID(uint32 uuid)
    {
        UUID = uuid;
    }
};