import json
from enum import Enum

from cbxp._C import call_cbxp

class CBXPErrorCode(Enum):
    """An enum of error and return codes from the cbxp interface"""
    COMMA_IN_INCLUDE = -1
    BAD_CONTROL_BLOCK = 1
    BAD_INCLUDE = 2

class CBXPError(Exception):
    """A class of errors for return codes from the cbxp interface"""
    def __init__(self, return_code: int, control_block_name: str):
        self.rc = return_code
        match self.rc:
            case CBXPErrorCode.COMMA_IN_INCLUDE.value:
                message = "Include patterns cannot contain commas"
            case CBXPErrorCode.BAD_CONTROL_BLOCK.value:
                message = f"Unknown control block '{control_block_name}' was specified."
            case CBXPErrorCode.BAD_INCLUDE.value:
                message = "A bad include pattern was provided"
            case _:
                message = "an unknown error occurred"
        super().__init__(message)

    
def cbxp(
        control_block: str,
        includes: list[str] = [],
        debug: bool = False
) -> dict:
    for include in includes:
        if "," in include:
            raise CBXPError(CBXPErrorCode.COMMA_IN_INCLUDE.value, control_block)
    response = call_cbxp(control_block.lower(), ",".join(includes).lower(), debug=debug)
    if response['return_code']:
        raise CBXPError(response['return_code'], control_block)
    return json.loads(response['result_json'])
