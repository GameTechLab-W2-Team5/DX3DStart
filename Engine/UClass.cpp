#include "stdafx.h"
#include "UClass.h"
#include "UObject.h"

IMPLEMENT_UCLASS(UClass, UObject)
UClass* UClass::RegisterToFactory(const std::string& typeName, const std::function<UObject* ()>& createFunction, const std::string& superClassTypeName)
{
	std::unique_ptr<UClass> classType = std::make_unique<UClass>();

	classType->typeId = registeredCount++;
	classType->className = typeName;
	classType->superClassTypeName = superClassTypeName;
	classType->createFunction = createFunction;

	nameToId[typeName] = classType->typeId;

	UClass* rawPtr = classType.get();
	classList.push_back(std::move(classType));
	return rawPtr;
}

void UClass::ResolveTypeBitsets()
{
	for (const std::unique_ptr<UClass>& _class : classList)
	{
        if (!_class->superClassTypeName.empty()) {
            auto it = nameToId.find(_class->superClassTypeName);
            _class->superClass = (it != nameToId.end()) ? classList[it->second].get() : nullptr;
        }
	}
    for (const std::unique_ptr<UClass>& _class : classList)
    {
        if (_class->processed) continue;

        _class->ResolveTypeBitset(_class.get());
    }
}

void UClass::ResolveTypeBitset(UClass* classPtr)
{
    std::stack<UClass*> stack;
    stack.push(classPtr);

    while (!stack.empty())
    {
        UClass* cur = stack.top();

        // 부모가 아직 처리되지 않았다면 먼저 스택에 push
        while (cur->superClass && !cur->superClass->processed)
        {
            stack.push(cur->superClass);
            cur = stack.top();
        }

        // 현재 노드 처리
        cur->typeBitset.Clear();
        if (cur->superClass) cur->typeBitset |= cur->superClass->typeBitset;
        cur->typeBitset.Set(cur->typeId);  // 자신의 비트 추가
        cur->processed = true;

        stack.pop();
    }
}


