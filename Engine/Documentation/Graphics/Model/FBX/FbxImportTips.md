# FbxImportTips

1. 获取Geometry的数量： `Scene->GetGeometryCount()`,`Scene->GetGeometry(GeoIndex)`  
2. 获取每个Node节点的唯一ID：`FbxObject::GetUniqueID` 
3. 获取材质时获取的是材质的index ID，所以要调用GetIndexArray(); 