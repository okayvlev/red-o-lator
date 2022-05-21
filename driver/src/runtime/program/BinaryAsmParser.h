#pragma once

#include <memory>
#include <string>
#include <utility>

#include "runtime/icd/CLKernel.h"

struct BinaryDisassemblingResult {
    std::string gpuName;
    std::string compileOptions;
    std::vector<std::string> parameters{};
    std::vector<std::unique_ptr<CLKernelBuilder>> kernelBuilders =
        std::vector<std::unique_ptr<CLKernelBuilder>>();

    std::string rawOutput;
};

class BinaryAsmParser {
   public:
    explicit BinaryAsmParser(std::shared_ptr<std::string> input)
        : input(std::move(input)) {}

    std::unique_ptr<BinaryDisassemblingResult> parseAsm();

   private:
    enum ParsingState { BinaryParameters, KernelConfig, KernelInstructions };

    void parseSingleInstruction(const std::string& instruction);

    void parseParameter(const std::string& line,
                        const std::string& parameterName,
                        const std::string& parameterValue);

    void parseBinaryParameter(const std::string& line,
                              const std::string& parameterName,
                              const std::string& parameterValue);

    void parseKernelConfigParameter(const std::string& line,
                                    const std::string& parameterName,
                                    const std::string& parameterValue);

    void parseKernelArgument(const std::string& argumentConfigLine);

    void parseKernelInstruction(const std::string& line,
                                const std::string& address,
                                const std::string& instruction,
                                const std::string& values);

    const std::shared_ptr<std::string> input;

    bool alreadyParsed = false;

    std::unique_ptr<BinaryDisassemblingResult> parsingResult = nullptr;
    ParsingState parsingState = BinaryParameters;

    std::unique_ptr<CLKernelBuilder> currentKernelBuilder = nullptr;
};
