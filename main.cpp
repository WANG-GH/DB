#include <iostream>
#include "skiplist.h"
#include "memtable.h"

int main() {
    const InternalKeyComparator internal_comparator_;
    MemTable * memtable = new MemTable(internal_comparator_);
    Slice key0("key0"),key1("key1"),value0("value0"),value1("value1"),key2("key2"),value2("value2");
    memtable->Add(0,kTypeValue,key0,value0);
    memtable->Add(1,kTypeValue,key1,value1);
    memtable->Add(2,kTypeValue,key2,value2);
    std::string value;
    Status s ;
    memtable->Get("key0",&value,&s);
    std::cout << value <<std::endl;
    memtable->Get("key2",&value,&s);
    std::cout << value <<std::endl;
    memtable->Get("key1",&value,&s);
    std::cout << value <<std::endl;
}
