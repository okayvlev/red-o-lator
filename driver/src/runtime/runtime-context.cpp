#include <common/utils/common.hpp>
#include <cstring>
#include <unordered_map>

#include "icd/CLContext.h"
#include "icd/CLDeviceId.hpp"
#include "runtime/common/runtime-commons.h"

cl_context createContext(const cl_context_properties* properties,
                         cl_device_id device,
                         CLContextCallback pfn_notify,
                         void* user_data,
                         cl_int* errcode_ret) {
    auto clContext = new CLContext(kDispatchTable, device);

    std::unordered_map<cl_context_properties, cl_context_properties>
        passedProperties;

    if (properties) {
        for (int i = 0; properties[i] != 0; i += 2) {
            passedProperties.emplace(properties[i], properties[i + 1]);
        }
        // TODO(clCreateContext, future): parse props
    }

    clContext->callback = utils::optionalOf(pfn_notify);
    clContext->callbackUserData = user_data;

    SET_SUCCESS();

    return clContext;
}

CL_API_ENTRY cl_context CL_API_CALL
clCreateContext(const cl_context_properties* properties,
                cl_uint num_devices,
                const cl_device_id* devices,
                CLContextCallback pfn_notify,
                void* user_data,
                cl_int* errcode_ret) {
    registerCall(__func__);

    if (!num_devices || !devices) {
        SET_ERROR_AND_RETURN(CL_INVALID_VALUE,
                             "Either num_devices == 0 or devices is null.");
    }

    if (!pfn_notify && user_data) {
        SET_ERROR_AND_RETURN(CL_INVALID_VALUE,
                             "user_data is set without pfn_notify.");
    }

    if (devices[0] != kDevice) {
        SET_ERROR_AND_RETURN(CL_INVALID_VALUE, "Invalid device.");
    }

    return createContext(properties, devices[0], pfn_notify, user_data,
                         errcode_ret);
}

CL_API_ENTRY cl_context CL_API_CALL
clCreateContextFromType(const cl_context_properties* properties,
                        cl_device_type device_type,
                        CLContextCallback pfn_notify,
                        void* user_data,
                        cl_int* errcode_ret) {
    registerCall(__func__);

    if (!kDevice) {
        clGetDeviceIDs(kPlatform, device_type, 0, nullptr, nullptr);
    }

    if (!kDevice->matchesType(device_type)) {
        if (errcode_ret) {
            *errcode_ret = CL_DEVICE_NOT_FOUND;
            return nullptr;
        }
    }

    return createContext(properties, kDevice, pfn_notify, user_data,
                         errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL clRetainContext(cl_context context) {
    registerCall(__func__);

    if (!context) {
        RETURN_ERROR(CL_INVALID_CONTEXT, "Context is null.");
    }

    context->referenceCount++;

    return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseContext(cl_context context) {
    registerCall(__func__);

    if (!context) {
        RETURN_ERROR(CL_INVALID_CONTEXT, "Context is null.");
    }

    context->referenceCount--;

    if (context->referenceCount == 0) {
        delete context;
    }

    return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL clGetContextInfo(cl_context context,
                                                 cl_context_info param_name,
                                                 size_t param_value_size,
                                                 void* param_value,
                                                 size_t* param_value_size_ret) {
    registerCall(__func__);

    if (!context) {
        RETURN_ERROR(CL_INVALID_CONTEXT, "Context is null.");
    }

    return getParamInfo(
        param_name, param_value_size, param_value, param_value_size_ret, [&]() {
            CLObjectInfoParameterValueType result;
            size_t resultSize;

            switch (param_name) {
                case CL_CONTEXT_REFERENCE_COUNT: {
                    resultSize = sizeof(cl_uint);
                    result = reinterpret_cast<void*>(context->referenceCount);
                    break;
                }

                case CL_CONTEXT_NUM_DEVICES: {
                    resultSize = sizeof(cl_uint);
                    result = reinterpret_cast<void*>(1);
                    break;
                }

                case CL_CONTEXT_DEVICES: {
                    resultSize = sizeof(cl_device_id);
                    result = kDevice;
                    break;
                }

                case CL_CONTEXT_PROPERTIES: {
                    // TODO(clGetContextInfo): parse props
                    resultSize = sizeof(cl_context_properties);
                    result = nullptr;
                    break;
                }

                default: return utils::optionalOf<CLObjectInfoParameterValue>();
            }

            return utils::optionalOf(
                CLObjectInfoParameterValue(result, resultSize));
        });
}
