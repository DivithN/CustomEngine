--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/PlainShader.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},

	Meshes =
	{
		{path = "Meshes/Ground.Divith"},
		{path = "Meshes/Mountains.Divith"},
		{path = "Meshes/ChestBase.Divith"},
		{path = "Meshes/ChestLid.Divith"},
		{path = "Meshes/ChestGold.Divith"},
		{path = "Meshes/Trees.Divith"},
		{path = "Meshes/House.Divith"},
		{path = "Meshes/Clouds.Divith"},
		{path = "Meshes/Clouds1.Divith"},
		{path = "Meshes/Swing.Divith"},
		{path = "Meshes/Car1.Divith"},
		{path = "Meshes/Car2.Divith"},
		{path = "Meshes/Car3.Divith"},
		{path = "Meshes/Car4.Divith"},
		{path = "Meshes/Car5.Divith"},
		{path = "Meshes/Car6.Divith"},
		{path = "Meshes/Car7.Divith"},
		{path = "Meshes/StopSign.Divith"},
		{path = "Meshes/Sun.Divith"},

	},

	Audio =
	{
		{path="Audio/AudioClips.json"},
		{path="Audio/metal_bottle.wav"},
		{path="Audio/piano.wav"},
		{path="Audio/traffic.wav"},
	}
}
