// This contains preprocessor flags that may be used to control how installation
// operations perform.

// Set if you want the driver to be installed with a single call to UpdateDriver, rather than
// a staged multipart operation using SetupCopyOEMInf.  The drawback is that an installation
// from a network location will fail if this flag is set.
// Recommended setting:  0
#define OCU_USE_UPDATEDRIVER			0