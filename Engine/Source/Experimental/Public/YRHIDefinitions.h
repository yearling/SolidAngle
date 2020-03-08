#pragma once

enum EYBufferUsageFlags
{
	// Mutually exclusive write-frequency flags
	YBUF_Static = 0x0001, // The buffer will be written to once.
	YBUF_Dynamic = 0x0002, // The buffer will be written to occasionally, GPU read only, CPU write only.  The data lifetime is until the next update, or the buffer is destroyed.
	YBUF_Volatile = 0x0004, // The buffer's data will have a lifetime of one frame.  It MUST be written to each frame, or a new one created each frame.

	// Mutually exclusive bind flags.
	YBUF_UnorderedAccess = 0x0008, // Allows an unordered access view to be created for the buffer.

	/** Create a byte address buffer, which is basically a structured buffer with a uint32 type. */
	YBUF_ByteAddressBuffer = 0x0020,
	/** Create a structured buffer with an atomic UAV counter. */
	YBUF_UAVCounter = 0x0040,
	/** Create a buffer that can be bound as a stream output target. */
	YBUF_StreamOutput = 0x0080,
	/** Create a buffer which contains the arguments used by DispatchIndirect or DrawIndirect. */
	YBUF_DrawIndirect = 0x0100,
	/**
	 * Create a buffer that can be bound as a shader resource.
	 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
	 */
	YBUF_ShaderResource = 0x0200,

	/**
	 * Request that this buffer is directly CPU accessible
	 * (@todo josh: this is probably temporary and will go away in a few months)
	 */
	YBUF_KeepCPUAccessible = 0x0400,

	/**
	 * Provide information that this buffer will contain only one vertex, which should be delivered to every primitive drawn.
	 * This is necessary for OpenGL implementations, which need to handle this case very differently (and can't handle GL_HALF_FLOAT in such vertices at all).
	 */
	YBUF_ZeroStride = 0x0800,

	/** Buffer should go in fast vram (hint only) */
	YBUF_FastVRAM = 0x1000,

	// Helper bit-masks
	YBUF_AnyDynamic = (YBUF_Dynamic | YBUF_Volatile),
};
