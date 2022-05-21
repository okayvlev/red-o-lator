#include <common/utils/optional-utlis.hpp>
#include <cstring>
#include <iostream>

#include "icd/CLDeviceId.hpp"
#include "runtime/common/runtime-commons.h"

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDs(cl_platform_id platform,
                                               cl_device_type device_type,
                                               cl_uint num_entries,
                                               cl_device_id* devices,
                                               cl_uint* num_devices) {
    registerCall(__func__);

    if (!platform || platform != kPlatform) {
        RETURN_ERROR(CL_INVALID_PLATFORM, "Platform is null or not valid.");
    }

    if (devices && num_entries == 0) {
        RETURN_ERROR(CL_INVALID_VALUE, "Devices is set but num_entries == 0.");
    }

    if (!kDevice) {
        const auto deviceConfigurationFile =
            std::getenv("RED_O_LATOR_DEVICE_CONFIG_PATH");

        if (!deviceConfigurationFile) {
            RETURN_ERROR(
                CL_INVALID_DEVICE,
                "Could not found device config path! Set "
                "RED_O_LATOR_DEVICE_CONFIG_PATH environment variable.");
        }

        try {
            kDeviceConfigurationParser.load(deviceConfigurationFile);

        } catch (const DeviceConfigurationParseError& e) {
            kLogger.error(e.what());

            return CL_INVALID_DEVICE;
        }

        kDevice = new CLDeviceId(
            kDispatchTable,
            kDeviceConfigurationParser.requireParameter<cl_device_type>(
                CL_DEVICE_TYPE),
            kDeviceConfigurationParser.requireParameter<size_t>(
                CL_DEVICE_GLOBAL_MEM_SIZE),
            kDeviceConfigurationParser.requireParameter<size_t>(
                CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE),
            kDeviceConfigurationParser.requireParameter<size_t>(
                CL_DEVICE_LOCAL_MEM_SIZE));
    }

    if (!kDevice->matchesType(device_type)) {
        return CL_DEVICE_NOT_FOUND;
    }

    if (devices) {
        devices[0] = kDevice;
    }

    if (num_devices) {
        *num_devices = 1;
    }

    return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceInfo(cl_device_id device,
                                                cl_device_info param_name,
                                                size_t param_value_size,
                                                void* param_value,
                                                size_t* param_value_size_ret) {
    registerCall(__func__);

    kLogger.temp("clGetDeviceInfo");

    if (device != kDevice) {
        RETURN_ERROR(CL_INVALID_DEVICE, "Device is null or not valid.");
    }

    // TODO(clGetDeviceInfo, future): parameters validation according to
    //  OpenCL spec

    return getParamInfo(
        param_name, param_value_size, param_value, param_value_size_ret, [&]() {
            if (param_name == CL_DEVICE_GLOBAL_FREE_MEMORY_AMD) {
                const auto freeMemKB =
                    (kDevice->globalMemorySize - kDevice->usedGlobalMemory) /
                    1024;
                return utils::optionalOf(CLObjectInfoParameterValue(
                    reinterpret_cast<void*>(freeMemKB), sizeof(size_t)));
            }

            auto param = kDeviceConfigurationParser.getParameter(param_name);

            if (param.has_value() &&
                std::holds_alternative<std::string>(param.value().value)) {
                kLogger.temp(std::to_string(param_name) + " -- " +
                             std::get<std::string>(param.value().value));
            } else {
                kLogger.temp(std::to_string(param_name) +
                             (param.has_value() ? "!" : ""));
            }

            return param;
        });
}

CL_API_ENTRY cl_int CL_API_CALL
clCreateSubDevices(cl_device_id in_device,
                   const cl_device_partition_property* properties,
                   cl_uint num_devices,
                   cl_device_id* out_devices,
                   cl_uint* num_devices_ret) {
    registerCall(__func__);

    // TODO(clCreateSubDevices, future): sub-devices support
    kLogger.warn("clCreateSubDevices is not supported!");

    if (num_devices > 0) {
        return CL_INVALID_DEVICE_PARTITION_COUNT;
    }

    if (num_devices_ret) {
        *num_devices_ret = 0;
    }

    return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL clRetainDevice(cl_device_id device) {
    registerCall(__func__);

    // TODO(clRetainDevice, future): sub-devices support
    if (device == kDevice) {
        return CL_SUCCESS;
    }

    return CL_INVALID_DEVICE;
}

CL_API_ENTRY cl_int CL_API_CALL clReleaseDevice(cl_device_id device) {
    registerCall(__func__);

    // TODO(clReleaseDevice, future): sub-devices support
    if (device == kDevice) {
        return CL_SUCCESS;
    }

    return CL_INVALID_DEVICE;
}
