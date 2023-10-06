#ifndef FAIL_CODE_H
#define FAIL_CODE_H

#define PENDING_CODE -1

enum FailureCodes{
    SUCCESS,
    FAILED_TO_LOAD_BIOS,
    FAILED_TO_DECODE,
    FAILED_TO_DECODE_ALU,
    FAILED_DECODED_TO_STRING,
    FAILED_TO_EXECUTE,
    FAILED_DMA,
};

#endif