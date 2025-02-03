#include <cstdlib>  // 用於 getenv()
#include <memory>
#include <stdexcept>
#include "IDisassembler.h"
#include "CapStone.h"
#include "BeaEngine.h"
#include "../../Env.h"

class Disassembler {
public:
    static IDisassembler& Instance() {
        static std::unique_ptr<IDisassembler> instance = CreateDisassembler();
        return *instance;
    }

private:
    static std::unique_ptr<IDisassembler> CreateDisassembler() {
        std::string type = Const::DisassemblerMode;
        if (type == "Capstone") {
            return std::make_unique<CapStoneDisassembler>();
        }
        else if (type == "BeaEngine") {
            return std::make_unique<BeaEngineDisassembler>();
        }
        else {
            throw std::runtime_error("不支援的反組譯器類型: " + type);
        }
    }
};