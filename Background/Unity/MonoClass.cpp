#include "MonoClass.h"



int MonoField::GetFlags()
{
	if (!Flags)
		Flags = Class->ClassAPI->FunctSet->FunctPtrSet["mono_field_get_flags"]->Call<int>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return Flags;
}

DWORD_PTR MonoField::GetTypeAddress()
{
	if (!TypeAddress)
		TypeAddress = Class->ClassAPI->FunctSet->FunctPtrSet["mono_field_get_type"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return TypeAddress;
}

std::string MonoField::GetTypeName()
{
	if (TypeName.empty())
		TypeName = Class->ClassAPI->FunctSet->FunctPtrSet["mono_type_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, GetTypeAddress());
	return TypeName;
}

DWORD_PTR MonoField::GetAddress()
{
	if (!Address)
		Address = Class->ClassAPI->FunctSet->FunctPtrSet["get_static_field_address"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return Address;
}

DWORD_PTR MonoField::GetValue()
{
	if (Class->ClassAPI->IsStatic(this))
		return Class->ClassAPI->GetStaticFieldValue<DWORD_PTR>(Class, this, GetTypeName());
	else
		return MonoUtils.ReadValue<DWORD_PTR>(FieldTypeNameMap[GetTypeName()], GetAddress());
}










MonoField* MonoClass::FindField(std::string FieldName)
{
	return ClassAPI->FindFieldInClassByName(this, FieldName);
}

DWORD_PTR MonoClass::GetVtable()
{
	if (!VTable)
		VTable = ClassAPI->FunctSet->FunctPtrSet["mono_class_vtable"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ClassAPI->ThreadFunctionList, ClassAPI->ThreadFunctionList->at(0), this->Handle);
	return VTable;
}









MonoClass* MonoClassAPI::FindClassInImageByName(std::string ImageName, std::string ClassName)
{
	MonoImage* Image = ImageAPI->FindImageByName(ImageName);
	if (Image) {
		// String Preprocess
		std::string ClassNamespace = "";
		size_t dot = ClassName.rfind('.');
		if (dot != std::string::npos) {
			ClassNamespace = ClassName.substr(0, dot);
			ClassName = ClassName.substr(dot + 1);
		}

		std::ranges::replace(ClassName, '+', '/'); // ±N '+' ´À´«¬° '/'

		// Get Class
		CString ClassNameObject(ClassName);
		CString ClassNamespaceObject(ClassNamespace);
		DWORD_PTR ClassAddress = 0x0;
		if (MonoNativeFuncSet::NativeFunctionExist(FunctSet, "mono_class_from_name_case"))
			ClassAddress = FunctSet->FunctPtrSet["mono_class_from_name_case"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Image->Handle, ClassNamespaceObject.Address, ClassNameObject.Address);
		if (!ClassAddress)
			ClassAddress = FunctSet->FunctPtrSet["mono_class_from_name"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Image->Handle, ClassNamespaceObject.Address, ClassNameObject.Address);
		ClassAddress &= 0xFFFFFFFFFFFF; // 12 bytes

		if (ClassAddress)
			return new MonoClass(this, Image, ClassAddress, ClassNameObject.Value, ClassNamespaceObject.Value);
		else
			return nullptr;
	}
	return nullptr;
}

MonoField* MonoClassAPI::FindFieldInClassByName(MonoClass* Class, std::string FieldName)
{
	CString FieldNameObject(FieldName);
	DWORD_PTR FieldAddress = FunctSet->FunctPtrSet["mono_class_get_field_from_name"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Class->Handle, FieldNameObject.Address);
	MonoField* FieldObj = new MonoField(Class, FieldAddress, FieldName);
	return FieldObj;
}

DWORD_PTR MonoClassAPI::GetStaticFieldAddress(MonoClass* Class, MonoField* Field)
{
	DWORD_PTR vtable = Class->GetVtable();
	if (vtable) {
		DWORD_PTR StaticFieldData = FunctSet->FunctPtrSet["mono_vtable_get_static_field_data"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, vtable);
		if (StaticFieldData) {
			int Offset = FunctSet->FunctPtrSet["mono_field_get_offset"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Field->Handle);
			return StaticFieldData + Offset;
		}
	}
	return 0x0;
}

template <typename T>
T MonoClassAPI::GetStaticFieldValue(MonoClass* Class, MonoField* Field, std::string TypeName)
{
	DWORD_PTR Address = GetStaticFieldAddress(Class, Field);
	if (Address)
		if (FieldTypeNameMap.find(TypeName) != FieldTypeNameMap.end())
			return MonoUtils.ReadValue<T>(FieldTypeNameMap[TypeName], Address);
		else
			return MonoUtils.ReadValue<DWORD_PTR>(TYPE_VOID_P, Address);
	return T();
}

bool MonoClassAPI::IsStatic(MonoField* Field)
{
	if (!Field) return false;
	return (Field->GetFlags() & (FIELD_ATTRIBUTE_STATIC | FIELD_ATTRIBUTE_HAS_FIELD_RVA)) != 0;
}