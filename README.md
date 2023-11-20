# BD1Manager
X Operations' BD1 File Manager

**EXAMPLE**
```cpp
#include <iostream>
#include "BD1Manager.hpp"

int main() {
    BD1Manager bd1;
    if (not bd1.LoadBD1("temp.bd1")) {
        std::cout << "Failed!\n";
        return -1;
    }

    if (not bd1.ExportMQO("temp.mqo")) {
        std::cout << "Failed!!\n";
        return -1;
    }

    std::cout << "Successed!\n";
    return 0;
}
```
