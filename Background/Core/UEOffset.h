#pragma once
#include <map>
#include <string>
#include "../../System/Process.h"

inline std::map<std::string, int> UETypeTable = {
    {"BoolProperty", 1},
    {"ByteProperty", 1},
    {"FloatProperty", 4},
    {"StructProperty", ProcessInfo::ProcOffestAdd},     //�j�p���w�A�O��
    {"IntProperty", 4},
    {"NameProperty", ProcessInfo::ProcOffestAdd},
    {"Int64Property", 8},
    {"TextProperty", ProcessInfo::ProcOffestAdd},
    {"StrProperty", ProcessInfo::ProcOffestAdd},
    {"ArrayProperty", ProcessInfo::ProcOffestAdd},
    {"MapProperty", ProcessInfo::ProcOffestAdd},
    {"ClassProperty", ProcessInfo::ProcOffestAdd},
    {"ObjectProperty", ProcessInfo::ProcOffestAdd}
};

class UEOffsetConfig
{
public:
    std::map<std::string, bool> UEOffestIsExist = {
        {"ID", true},     //
        {"Class", true},        //
        {"FNameIndex", true},   //
        {"Outer", true},        //

        {"Super", true},        //
        {"Member", true},      //
        {"MemberFNameIndex", false},      //
        {"NextMember", true},  //
        {"Offset", true},      //
        {"PropSize", true},    //
        {"Property", true},    //

        {"BitMask", true},     //

        {"EnumList", true},
        {"EnumPropMul", true},
        {"EnumPropName", true},
        {"EnumPropIndex", true},

        {"Funct", true}
    };

    size_t ID = 0xC - ProcessInfo::ProcOffestSub;
    size_t Class = 0x10 - ProcessInfo::ProcOffestSub;                //Class �� Outer �N�O Package
    size_t FNameIndex = 0x18 - (ProcessInfo::ProcOffestSub * 2);
    size_t Outer = 0x20 - (ProcessInfo::ProcOffestSub * 2);

    size_t Super = 0x40;     //Object        =>Super    //class => 0x88 
    size_t Member = 0x50;     //Object        =>Member
    size_t MemberSize = 0x58;     //Object        =>MemberSize            //���ϥ�
    size_t MemberTypeOffest = 0x8;      //Object        =>MemberTypeOffest
    size_t MemberType = 0x0;      //ObjectType    =>MemberType
    size_t MemberSubType = 0x8;      //ObjectType    =>MemberSubType ?       //���ϥ�
    size_t MemberFNameIndex = 0x20;     //Object        =>MemberFNameIndex      //0x28      0x20(UE5)
    size_t MemberList = 0xA8;     //Object        =>MemberList            //���ϥ�   //�ѼƩM�~�Ӫ�Class�����ɡAMember �|�ܦ� function �� List�A���ܼƭn�b�o��~�䪺��
    size_t MemberListSize = 0x0;      //MemberList    =>MemberListSize        //���ϥ�
    size_t NextMember = 0x18;     //Member        =>NextMember            //0x20      0x18(UE5)      
    size_t NextMember_SameClass = 0x50;     //NextMember's Option                   //���ϥ�
    size_t NextMember_AllUsed = 0x18;     //NextMember's Option                   //���ϥ�   //�򥻤W�p�G�n���j��A�N�n���o���ѼƤ~�|�����A���M�N�����q MemberList
    size_t Offset = 0x44;     //Object        =>Offset                //0x4C      0x44(UE5)
    size_t PropSize = 0x34;     //Object        =>PropSize              //0x3C      0x34(UE5)
    size_t Property = 0x78;     //Object        =>Property
    size_t TypeObject = 0x70;     //Object        =>TypeObject            //Type �p�G�O Object �h��ܸ� Object�AEx: Map<KeyObject, ValueObject> �|��� KeyObject�A�� ValueObject �A�Ĥ@�� Property ��
    size_t BitMask = 0x72;     //Object        =>BitMask               //0x7A      0x72(UE5)        //EX : 0404  => 010000000100 (�ݳ̥k�䪺 bit�A�q 0 �}�l�Ʀb�ĤG�ӡA�ҥH�O�� bool �ܼƭn�x�s�b�ĤG�� bit)


    // EnumProperty ���� UEOffset.Property �� Propty 
    // => ���o Enum Object �A���� UEOffset.EnumList 
    // => ���o EnumList ���ۻ��j UEOffset.EnumSize ���a���j�p 
    // => �����Ҧ� Enum �����e
    size_t EnumName = 0x30;     //Enum          =>EnumName              //Unicode
    size_t EnumList = 0x40;     //Enum          =>EnumList
    size_t EnumSize = 0x48;     //Enum          =>EnumSize              //���ϥ�
    size_t EnumPropName = 0x0;      //EnumList      =>EnumPropName
    size_t EnumPropIndex = 0x8;      //EnumList      =>EnumPropIndex
    size_t EnumPropMul = 0x10;     //EnumList      =>EnumPropMul
    size_t EnumType = 0x70;     //EnumProperty  => EnumType             //���ϥ�   //��� Enum �� Type (����x�s��)�AEX: ByteProperty ��ܥΤ@�� Byte �h�x�s�C�ӿﶵ

    // ArrayProperty ���� EOffset.Property �� Propty 
    // => ���o StructProperty �A���� EOffset.Property �� Propty 
    // => ���o Struct Object  ���۴N�h���j Member
    size_t Array = 0x70;     //ArrayProperty =>ArrayOffest           //���ϥ�            //���o Array Object �̭��� Object�A EX Array<MyObject> �����X MyObject
    size_t MapKey = 0x70;     //EnumProperty  =>MapKeyOffest
    size_t MapValue = 0x70;     //EnumProperty  =>Propert�A             //���ϥ�
    //�A =>MapValueOffest
    size_t StructName = 0x70;                                             //���ϥ�

    size_t Funct = 0xD8;     //Function Address       //0x70
    size_t FunctPara = Member;                                                      //���ϥ�
    size_t FunctClass = Outer;                                                      //���ϥ�
    size_t NextPara = 0x48;
    size_t ParaType = 0x70;                                                         //���ϥ�

private:
};

inline UEOffsetConfig UEOffset = UEOffsetConfig();