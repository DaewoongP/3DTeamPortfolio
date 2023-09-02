attrib				-R			.\Reference\Headers\*.*
xcopy				/y /s		.\Engine\Public\*.*					.\Reference\Headers\
attrib				+R			.\Reference\Headers\*.*

xcopy				/y /s		.\Engine\ThirdPartyLib\*.*			.\Reference\Librarys\

xcopy				/y			.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Tool\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Assimp\Bin\

xcopy				/y			.\Engine\Bin\Engine_d.lib			.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Tool\Bin\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Assimp\Bin\

xcopy				/y			.\Engine\Bin\ShaderFiles\*.hlsl		.\Client\Bin\ShaderFiles\
xcopy				/y			.\Engine\Bin\ShaderFiles\*.hlsl		.\Tool\Bin\ShaderFiles\
xcopy				/y			.\Engine\Bin\ShaderFiles\*.hlsli	.\Client\Bin\ShaderFiles\
xcopy				/y			.\Engine\Bin\ShaderFiles\*.hlsli	.\Tool\Bin\ShaderFiles\

xcopy				/y			.\Engine\Bin\fmod.dll				.\Client\Bin\
xcopy				/y			.\Engine\Bin\fmod.dll				.\Tool\Bin\
xcopy				/y			.\Engine\Bin\fmod.dll				.\Assimp\Bin\

xcopy				/y			.\Engine\Bin\*.dll			.\Client\Bin\
xcopy				/y			.\Engine\Bin\*.dll			.\Tool\Bin\


xcopy				/y			.\Engine\Bin\*.dll			.\Assimp\Bin\

