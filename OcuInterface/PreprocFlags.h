#pragma once

/// This flag should be set to 0 if the interface should never try to send more
/// that one input in an individual input report, effectively forcing the input
/// mode to serial.  This debugging flag is useful if you are trying to identify
/// defects in the hybrid packet format.
///
/// Recommended value:  1
#define OCUINT_ALLOW_HYBRID_SYNTHESIS		0