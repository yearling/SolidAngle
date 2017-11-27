# DebugLayer bugs
1. `D3D11 WARNING: ID3D11DeviceContext::DrawIndexed: Index buffer has not enough space! [ EXECUTION WARNING #359: DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL]` 
    
	DrawIndex()中，要画的三角形的index数量大于实际IB的数量 