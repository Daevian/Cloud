

function testest(a0, a1, a2, a3)
	print(a0)
    print(a1)
    print(a2)
    print(a3)
    test = TestFunction(24.45, 78)
    print(test)
    test = TestFunction2(55.55)
    print(test)
    test = TestFunction3()
    print(test)
    return 0.2, 56
end

function render()
	backbuffer = GetResource("backbuffer")
    depth = GetResource("depth")
    
    GfxClearColour(backbuffer)
    GfxClearDepth(depth)
    GfxSetRenderTarget(backbuffer, depth)
end


