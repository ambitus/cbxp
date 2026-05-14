def call_cbxp_extract(  # noqa: N999
    control_block_name: str,
    includes_string: str,
    filters_string: str,
    debug: bool = False,
) -> dict: ...
def call_cbxp_format(  # noqa: N999
    control_block_name: str,
    data: bytes,
    offset: int = 0,
    debug: bool = False,
) -> dict: ...
