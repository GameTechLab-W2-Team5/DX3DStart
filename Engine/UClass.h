#pragma once
#include "UEngineStatics.h"
#include "FDynamicBitset.h"
#include "json.hpp"
#include "UObject.h"
#include <memory>

class UClass : public UObject
{
	DECLARE_UCLASS(UClass, UObject)
private:
	static inline std::vector<std::unique_ptr<UClass>> classList;
	static inline std::unordered_map<std::string, uint32> nameToId;
	static inline std::unordered_map<std::string, uint32> displayNameToId;
	static inline uint32 registeredCount = 0;

	std::unordered_map<std::string, std::string> metadata;
	uint32 typeId;
	FDynamicBitset typeBitset;
	std::string className, superClassTypeName;
	UClass* superClass;
	std::function<UObject*()> createFunction;
	bool processed = false;
public:
	static UClass* RegisterToFactory(const std::string& typeName, 
		const std::function<UObject*()>& createFunction, const std::string& superClassTypeName);

	static void ResolveTypeBitsets();
	void ResolveTypeBitset(UClass* classPtr);

	static UClass* GetClass(uint32 typeId) {
		return (typeId < classList.size()) ? classList[typeId].get() : nullptr;
	}

	static UClass* FindClass(const std::string& name) {
		auto it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}

	static UClass* FindClassWithDisplayName(const std::string& name)
	{
		// 1) DisplayName lookup
		auto it = displayNameToId.find(name);
		if (it != displayNameToId.end())
			return GetClass(it->second);

		// 2) className fallback
		it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}


	static const TArray<std::unique_ptr<UClass>>& GetClassList()
	{
		return classList;
	}

	static void CleanupClassRegistry()
	{
		char buffer[256];
		sprintf_s(buffer, "CleanupClassRegistry called: %zu classes registered\n", classList.size());
		OutputDebugStringA(buffer);
		
		// UClass 객체들이 UObject 배열에서도 제거되도록 수동으로 소멸자 호출
		for (auto& classPtr : classList) {
			sprintf_s(buffer, "Manually destroying UClass: %s\n", classPtr->className.c_str());
			OutputDebugStringA(buffer);
		}
		
		classList.clear();  // unique_ptr의 소멸자가 호출되어 UObject 소멸자도 호출됨
		nameToId.clear();
		displayNameToId.clear();
		registeredCount = 0;
		
		OutputDebugStringA("CleanupClassRegistry completed\n");
	}

	bool IsChildOrSelfOf(UClass* baseClass) const {
		return baseClass && typeBitset.Test(baseClass->typeId);
	}

	const std::string& GetUClassName() const { return className; }

	const std::string& GetDisplayName() const
	{
		auto itr = metadata.find("DisplayName");
		if (itr != metadata.end())
		{
			return itr->second;
		}

		return GetUClassName();
	}

	void SetMeta(const std::string& key, const std::string& value)
	{
		metadata[key] = value;

		if (key == "DisplayName")
		{
			displayNameToId[value] = typeId;  // typeId는 인스턴스 멤버
		}
	}


	const std::string& GetMeta(const std::string& key) const {
		static std::string empty;
		auto it = metadata.find(key);
		return (it != metadata.end()) ? it->second : empty;
	}

	UObject* CreateDefaultObject() const {
		return createFunction ? createFunction() : nullptr;
	}

	template<typename T>
	TUniquePtr<T> CreateUniqueObject() const {
		UObject* obj = CreateDefaultObject();
		if (obj) {
			T* typedObj = obj->Cast<T>();
			if (typedObj) {
				return TUniquePtr<T>(typedObj);
			} else {
				delete obj; // Cast 실패 시 메모리 해제
			}
		}
		return nullptr;
	}

};


