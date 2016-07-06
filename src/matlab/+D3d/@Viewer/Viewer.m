classdef (Abstract,Sealed) Viewer
methods (Static)
    commandInfo = Info()
    Help(command)
    AddPolygons(polygonsStruct)
    CaptureImage()
    CaptureSpinMovie()
    Close()
    DeleteAllPolygons()
    DisplayPolygons()
    Init(arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr)
    LoadTexture(Image,AnisotropicVoxelDimension,BufferType)
    PeelUpdate(PeelSize)
    Play()
    MessageArray = Poll()
    ReleaseControl()
    RemovePolygon()
    ResetView()
    Rotate()
    PolygonLighting()
    SetBackgroundColor()
    SetCapturePath()
    SetFrame()
    SetViewOrigin()
    ShowLabels()
    ShowPolygons(polygonIdxList)
    ShowTexture(bufferType)
    TakeControl()
    TextureAttenuation()
    TextureLighting()
    TransferFunction(TransferFunctionStruct,BufferType)
    ToggleWireframe(on)
end
methods (Static, Access = private)
    varargout = Mex(command, varargin)
end
end
