#pragma once

// Version information used everywhere
#define OCS_MAJ(x)				(((x) & 0xF0000000) >> 28)
#define OCS_MIN(x)				(((x) & 0x0F000000) >> 24)
#define OCS_REV(x)				(((x) & 0x00FFFFFF) >> 0)

#define OCS_VER(maj, min, rev)	(((maj) << 28) | ((min) << 24) | rev)

#define HIDEMULATOR_VERSION		OCS_VER(0, 8, 2)
