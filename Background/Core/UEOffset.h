#pragma once
#include <map>
#include <string>
#include "../../System/Process.h"

inline std::map<std::string, int> UETypeTable = {
    {"BoolProperty", 1},
    {"ByteProperty", 1},
    {"FloatProperty", 4},
    {"StructProperty", ProcessInfo::ProcOffestAdd},     //大小不定，別用
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
    size_t Class = 0x10 - ProcessInfo::ProcOffestSub;                //Class 的 Outer 就是 Package
    size_t FNameIndex = 0x18 - (ProcessInfo::ProcOffestSub * 2);
    size_t Outer = 0x20 - (ProcessInfo::ProcOffestSub * 2);

    size_t Super = 0x40;     //Object        =>Super    //class => 0x88 
    size_t Member = 0x50;     //Object        =>Member
    size_t MemberSize = 0x58;     //Object        =>MemberSize            //未使用
    size_t MemberTypeOffest = 0x8;      //Object        =>MemberTypeOffest
    size_t MemberType = 0x0;      //ObjectType    =>MemberType
    size_t MemberSubType = 0x8;      //ObjectType    =>MemberSubType ?       //未使用
    size_t MemberFNameIndex = 0x20;     //Object        =>MemberFNameIndex      //0x28      0x20(UE5)
    size_t MemberList = 0xA8;     //Object        =>MemberList            //未使用   //參數和繼承的Class有關時，Member 會變成 function 的 List，而變數要在這邊才找的到
    size_t MemberListSize = 0x0;      //MemberList    =>MemberListSize        //未使用
    size_t NextMember = 0x18;     //Member        =>NextMember            //0x20      0x18(UE5)      
    size_t NextMember_SameClass = 0x50;     //NextMember's Option                   //未使用
    size_t NextMember_AllUsed = 0x18;     //NextMember's Option                   //未使用   //基本上如果要遞迴找，就要走這條參數才會齊全，不然就直接從 MemberList
    size_t Offset = 0x44;     //Object        =>Offset                //0x4C      0x44(UE5)
    size_t PropSize = 0x34;     //Object        =>PropSize              //0x3C      0x34(UE5)
    size_t Property = 0x78;     //Object        =>Property
    size_t TypeObject = 0x70;     //Object        =>TypeObject            //Type 如果是 Object 則表示該 Object，Ex: Map<KeyObject, ValueObject> 會表示 KeyObject，而 ValueObject 再第一個 Property 內
    size_t BitMask = 0x72;     //Object        =>BitMask               //0x7A      0x72(UE5)        //EX : 0404  => 010000000100 (看最右邊的 bit，從 0 開始數在第二個，所以是該 bool 變數要儲存在第二個 bit)


    // EnumProperty 偏移 UEOffset.Property 到 Propty 
    // => 取得 Enum Object 再偏移 UEOffset.EnumList 
    // => 取得 EnumList 接著遞迴 UEOffset.EnumSize 取地的大小 
    // => 取的所有 Enum 的內容
    size_t EnumName = 0x30;     //Enum          =>EnumName              //Unicode
    size_t EnumList = 0x40;     //Enum          =>EnumList
    size_t EnumSize = 0x48;     //Enum          =>EnumSize              //未使用
    size_t EnumPropName = 0x0;      //EnumList      =>EnumPropName
    size_t EnumPropIndex = 0x8;      //EnumList      =>EnumPropIndex
    size_t EnumPropMul = 0x10;     //EnumList      =>EnumPropMul
    size_t EnumType = 0x70;     //EnumProperty  => EnumType             //未使用   //顯示 Enum 的 Type (怎麼儲存的)，EX: ByteProperty 表示用一個 Byte 去儲存每個選項

    // ArrayProperty 偏移 EOffset.Property 到 Propty 
    // => 取得 StructProperty 再偏移 EOffset.Property 到 Propty 
    // => 取得 Struct Object  接著就去遞迴 Member
    size_t Array = 0x70;     //ArrayProperty =>ArrayOffest           //未使用            //取得 Array Object 裡面的 Object， EX Array<MyObject> 中取出 MyObject
    size_t MapKey = 0x70;     //EnumProperty  =>MapKeyOffest
    size_t MapValue = 0x70;     //EnumProperty  =>Propert，             //未使用
    //再 =>MapValueOffest
    size_t StructName = 0x70;                                             //未使用

    size_t Funct = 0xD8;     //Function Address       //0x70
    size_t FunctPara = Member;                                                      //未使用
    size_t FunctClass = Outer;                                                      //未使用
    size_t NextPara = 0x48;
    size_t ParaType = 0x70;                                                         //未使用

private:
};

inline UEOffsetConfig UEOffset = UEOffsetConfig();