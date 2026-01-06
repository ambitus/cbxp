import json
from enum import Enum

from cbxp._C import call_cbxp


class CBXPErrorCode(Enum):
    """An enum of error and return codes from the cbxp interface"""

    COMMA_IN_INCLUDE = -1
    COMMA_IN_FILTER = -2
    NO_FILTER_MATCH = -3
    BAD_CONTROL_BLOCK = 1
    BAD_INCLUDE = 2
    BAD_CONTROL_BLOCK_FILTER = 3


class CBXPError(Exception):
    """A class of errors for return codes from the cbxp interface"""

    def __init__(self, return_code: int, control_block_name: str):
        self.rc = return_code
        match self.rc:
            case CBXPErrorCode.COMMA_IN_INCLUDE.value:
                message = "Include patterns cannot contain commas"
            case CBXPErrorCode.COMMA_IN_FILTER.value:
                message = "Filters cannot contain commas"
            case CBXPErrorCode.BAD_CONTROL_BLOCK.value:
                message = f"Unknown control block '{control_block_name}' was specified."
            case CBXPErrorCode.BAD_INCLUDE.value:
                message = "A bad include pattern was provided"
            case CBXPErrorCode.BAD_CONTROL_BLOCK_FILTER.value:
                message = "A bad filter was provided"
            case CBXPErrorCode.NO_FILTER_MATCH.value:
                message = "No control block was found that matched the provided filter"
            case _:
                message = "an unknown error occurred"
        super().__init__(message)


def cbxp(
    control_block: str,
    includes: list[str] = None,
    control_block_filters: list[str] = None,
    debug: bool = False,
) -> dict:
    # Includes processing
    if includes is None:
        includes = []
    for include in includes:
        if "," in include:
            raise CBXPError(CBXPErrorCode.COMMA_IN_INCLUDE.value, control_block)

    # Filter Processing
    if control_block_filters is None:
        control_block_filters = []
    for control_block_filter in control_block_filters:
        if "," in control_block_filter:
            raise CBXPError(CBXPErrorCode.COMMA_IN_FILTER.value, control_block)

    response = call_cbxp(
        control_block.lower(),
        ",".join(includes).lower(),
        ",".join(control_block_filters).lower(),
        debug=debug,
    )
    if response["return_code"]:
        raise CBXPError(response["return_code"], control_block)
    if response["result_json"] == "null" and control_block_filters != []:
        raise CBXPError(CBXPErrorCode.NO_FILTER_MATCH.value, control_block)
    return json.loads(response["result_json"])
