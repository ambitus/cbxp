import json
from enum import Enum

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
    OFFSET_TOO_BIG = -3
    OFFSET_NEGATIVE = -4
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
            case CBXPErrorCode.OFFSET_TOO_BIG.value:
                message = "Offset is too large for data provided"
            case CBXPErrorCode.OFFSET_NEGATIVE.value:
                message = "Offset parameter can not be negative"
            case CBXPErrorCode.BAD_CONTROL_BLOCK.value:
                message = f"Unknown control block '{control_block_name}' was specified."
            case CBXPErrorCode.BAD_INCLUDE.value:
                message = "A bad include pattern was provided"
            case CBXPErrorCode.BAD_CONTROL_BLOCK_FILTER.value:
                message = "A bad filter was provided"
            case CBXPErrorCode.BUFFER_TOO_SMALL.value:
                message = (
                    "The buffer is not large enough to contain a "
                    f"'{control_block_name}' control block"
                )
            case _:
                message = "an unknown error occurred"
        super().__init__(message)


def extract(
    control_block: str,
    includes: list[str] = None,
    filters: list[CBXPFilter] = None,
    debug: bool = False,
) -> dict:
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
    if response["return_code"]:
        raise CBXPError(response["return_code"], control_block)
    if response["result_json"] == "null" or response["result_json"] == "[]":
        return None
    return json.loads(response["result_json"])


def format(  # noqa: A001
    control_block: str,
    data: bytes,
    offset: int = None,
    debug: bool = False,
) -> dict:
    if offset is None:
        offset = 0
    elif offset < 0:
        raise CBXPError(CBXPErrorCode.OFFSET_NEGATIVE.value, control_block)
    elif offset >= len(data):
        raise CBXPError(CBXPErrorCode.OFFSET_TOO_BIG.value, control_block)

    response = call_cbxp_format(
        control_block.lower(),
        data,
        offset=offset,
        debug=debug,
    )
    if response["return_code"]:
        raise CBXPError(response["return_code"], control_block)
    if response["result_json"] == "null" or response["result_json"] == "[]":
        return None
    return json.loads(response["result_json"])
