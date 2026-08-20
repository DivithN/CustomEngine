--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/customShader.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/customShader2.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},

	Meshes =
	{
		{path = "Meshes/Plane.Divith"},
		{path = "Meshes/cube.Divith"},
		{path = "Meshes/Ball.Divith"},
		{path = "Meshes/House.Divith"},
		{path = "Meshes/Roof.Divith"},
		{path = "Meshes/Tree.Divith"},
		{path = "Meshes/Swing.Divith"},
		{path = "Meshes/Fence.Divith"},
		{path = "Meshes/BackGround.Divith"},

	},
}
