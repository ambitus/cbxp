import json
from enum import Enum

from zoau.datasets import Dataset

from cbxp._C import call_cbxp_extract, call_cbxp_format


class CBXPFilterOperation(Enum):
    """An enum of possible filter operations for the cbxp interface"""

    EQUAL = "="
    LESS_THAN = "<"
    GREATER_THAN = ">"
    LESS_THAN_OR_EQUAL = "<="
    GREATER_THAN_OR_EQUAL = ">="


class CBXPFilter:
    """A class to represent a filter to limit cbxp output based on set conditions"""

    def __init__(
        self,
        key: str,
        operation: CBXPFilterOperation,
        value: str | int,
    ):
        self.key = key
        self.operation = operation.value
        self.value = value

    def __str__(self):
        return str(self.key) + str(self.operation) + str(self.value)


class CBXPErrorCode(Enum):
    """An enum of error and return codes from the cbxp interface"""

    COMMA_IN_INCLUDE = -1
    COMMA_IN_FILTER = -2
    BAD_FORMAT_PARMS = -3
    BAD_EXPLORE_PARMS = -4
    BAD_OPERATION = -5
    MISSING_FORMAT_PARMS = -6
    OFFSET_TOO_BIG = -7
    BAD_CONTROL_BLOCK = 1
    BAD_INCLUDE = 2
    BAD_CONTROL_BLOCK_FILTER = 3
    BUFFER_TOO_SMALL = 4


class CBXPError(Exception):
    """A class of errors for return codes from the cbxp interface"""

    def __init__(self, return_code: int, control_block_name: str):
        self.rc = return_code
        match self.rc:
            case CBXPErrorCode.COMMA_IN_INCLUDE.value:
                message = "Include patterns cannot contain commas"
            case CBXPErrorCode.COMMA_IN_FILTER.value:
                message = "Filters cannot contain commas"
            case CBXPErrorCode.BAD_EXPLORE_PARMS.value:
                message = (
                    "'file_path', 'dataset_path', 'raw_data' and 'offset' parameters "
                    "cannot be used with the 'explore' operation"
                )
            case CBXPErrorCode.BAD_FORMAT_PARMS.value:
                message = (
                    "Filters and Includes cannot be used with the 'format' operation"
                )
            case CBXPErrorCode.BAD_OPERATION.value:
                message = "cbxp must perform 'format' or 'explore' operation"
            case CBXPErrorCode.MISSING_FORMAT_PARMS.value:
                message = (
                    "Exactly one of 'file_path', 'dataset_path' and 'raw_data' "
                    "parameters is required for 'format' operation"
                )
            case CBXPErrorCode.OFFSET_TOO_BIG.value:
                message = "Offset is too large for specified data/file"
            case CBXPErrorCode.BAD_CONTROL_BLOCK.value:
                message = f"Unknown control block '{control_block_name}' was specified."
            case CBXPErrorCode.BAD_INCLUDE.value:
                message = "A bad include pattern was provided"
            case CBXPErrorCode.BAD_CONTROL_BLOCK_FILTER.value:
                message = "A bad filter was provided"
            case CBXPErrorCode.BUFFER_TOO_SMALL.value:
                message = (
                    "The buffer is not large enough to contain a "
                    f"'{control_block_name}'"
                )
            case _:
                message = "an unknown error occurred"
        super().__init__(message)


def cbxp(
    control_block: str,
    operation: str = "explore",
    includes: list[str] = None,
    filters: list[CBXPFilter] = None,
    file_path: str = None,
    dataset_path: str = None,
    raw_data: bytes = None,
    offset: int = None,
    debug: bool = False,
) -> dict:
    if operation == "explore":
        if (
            file_path is not None
            or dataset_path is not None
            or offset is not None
            or raw_data is not None
        ):
            raise CBXPError(CBXPErrorCode.BAD_EXPLORE_PARMS.value, control_block)
        # Includes processing
        if includes is None:
            includes = []
        for include in includes:
            if "," in include:
                raise CBXPError(CBXPErrorCode.COMMA_IN_INCLUDE.value, control_block)

        # Filter Processing
        if filters is None:
            filters = []
        filters_string = ""
        for filter_obj in filters:
            if filters_string != "":
                filters_string += ","
            if "," in str(filter_obj):
                raise CBXPError(CBXPErrorCode.COMMA_IN_FILTER.value, control_block)
            filters_string += str(filter_obj)

        response = call_cbxp_extract(
            control_block.lower(),
            ",".join(includes),
            filters_string,
            debug=debug,
        )
    elif operation == "format":
        if filters is not None or includes is not None:
            raise CBXPError(CBXPErrorCode.BAD_FORMAT_PARMS.value, control_block)
        data_buffer = None
        if raw_data is not None and file_path is None and dataset_path is None:
            data_buffer = raw_data
        elif raw_data is None and file_path is not None and dataset_path is None:
            with open(file_path, "rb") as f:
                data_buffer = f.read()
        elif raw_data is None and file_path is None and dataset_path is not None:
            ds = Dataset(dataset_path)
            data_buffer = ds.read_as_bytes()
        else:
            raise CBXPError(CBXPErrorCode.MISSING_FORMAT_PARMS.value, control_block)

        if offset is None:
            offset = 0
        elif offset >= len(data_buffer):
            raise CBXPError(CBXPErrorCode.OFFSET_TOO_BIG.value, control_block)

        response = call_cbxp_format(
            control_block.lower(),
            data_buffer,
            offset,
            debug=debug,
        )

    else:
        raise CBXPError(CBXPErrorCode.BAD_OPERATION.value, control_block)
    if response["return_code"]:
        raise CBXPError(response["return_code"], control_block)
    if response["result_json"] == "null" or response["result_json"] == "[]":
        return None
    return json.loads(response["result_json"])
