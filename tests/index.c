#include "clar.h"

#include "stl.h"
#include "export.h"
#include "index.h"

char cube_path[] = CLAR_FIXTURE_PATH "cube.stl";
stl_object *cube_mesh = NULL;
klist_t(poly) *cube_mesh_polys = NULL;
mesh_index_t *idx = NULL;

void test_index__initialize(void) {
	cube_mesh = stl_read_file(cube_path, 1);
	cl_assert_(cube_mesh != NULL, "Failed to read cube");

	cube_mesh_polys = stl_to_polys(cube_mesh);
	cl_assert_(cube_mesh_polys != NULL, "Failed to export mesh as polygon list.");

	idx = alloc_mesh_index(cube_mesh_polys);
	cl_assert_(idx, "Failed to build index");
	cl_assert_(idx->polygons != NULL, "Poly list does not exist");
	cl_assert_equal_i(idx->polygons->size, cube_mesh_polys->size);
}

void test_index__cleanup(void) {
	if(cube_mesh != NULL) stl_free(cube_mesh);
	if(cube_mesh_polys != NULL) kl_destroy(poly, cube_mesh_polys);
	if(idx) free_mesh_index(idx);
	cube_mesh = NULL;
	cube_mesh_polys = NULL;
	idx = NULL;
}

void test_index__reduces_vertex_count(void) {
	cl_assert_(idx->vertex_tree != NULL, "Vertex tree does not exist");

	size_t count = 0;
	vertex_tree_walk(idx->vertex_tree, vertex_node_count, &count);
	// Assuming that there are three vertexes per polygon
	// since that's what the STL spec mandates
	cl_assert_(count < cube_mesh_polys->size * 3, "Duplicate verticies should be removed in the vertex index");
}

void test_index__edge_count(void) {
	cl_assert_(idx->edge_tree != NULL, "Edge tree does not exist.");

	size_t count = 0;
	edge_tree_walk(idx->edge_tree, edge_node_count, &count);
	// A cube expressed as triangles has 18 edges.
	cl_assert_equal_i(count, 18);
}
