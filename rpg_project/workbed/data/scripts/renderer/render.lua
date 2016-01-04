


backbuffer = GetResource("backbuffer")
depth = GetResource("depth")


GfxClearColour(backbuffer)
GfxClearDepth(depth)
GfxSetRenderTarget(backbuffer, depth)