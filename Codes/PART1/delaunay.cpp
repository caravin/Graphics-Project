#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <cassert>
#include <vector>
#include <map>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Triangulation;
typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
typedef Triangulation::Finite_edges_iterator Finite_edges_iterator;
typedef Triangulation::Finite_facets_iterator Finite_facets_iterator;
typedef Triangulation::Finite_cells_iterator Finite_cells_iterator;
typedef Triangulation::Simplex        Simplex;
typedef Triangulation::Locate_type    Locate_type;
typedef Triangulation::Point          Point;
int main()
{
  // construction from a list of points :
  
std::list<Point> L;
std::vector<double> Cluster;
std::vector<std::vector<double>> close_clusters;
std::map<Point,double> Mapper;
FILE* file = fopen("/home/ubuntu/graphics_20_clusters.txt", "r");
if (file == NULL) {
std::cout << "Couldn't read the file" << std::endl;
return false;
}
for(int count=0;count<502;count++){

//int line_no;
double x,y,z;
double cluster_no;
double close1,close2,close3,close4,close5;
std::vector<double> close;
int scan_return_value;
scan_return_value=fscanf(file, "%lf %lf %lf %lf %lf %lf %lf %lf %lf\n",&x, &y, &z, &cluster_no, &close1, &close2, &close3, &close4, &close5);
L.push_front(Point(x,y,z));
Mapper.insert({Point(x,y,z),cluster_no});
close.push_back(close1);
close.push_back(close2);
close.push_back(close3);
close.push_back(close4);
close.push_back(close5);
close_clusters.push_back(close);
/*std::cout<<Point(x,y,z)<<" "<<cluster_no<<" "<<close1<<" "<<close2<<" "<<close3<<" "<<close4<<" "<<close5<<" " <<std::endl;*/
}

  
  
  
  Triangulation T(L.begin(), L.end());
  std::set<Simplex> simplices;
  /*Finite_vertices_iterator vit = T.finite_vertices_begin();
  simplices.insert(Simplex(vit));
  Finite_cells_iterator cit = T.finite_cells_begin();
  simplices.insert(Simplex(cit));
  Finite_edges_iterator eit = T.finite_edges_begin();
  simplices.insert(Simplex(*eit));*/
  //Finite_facets_iterator fit = T.finite_facets_begin();

    Triangulation::Finite_edges_iterator iter;
    for(iter =  T.finite_edges_begin();
        iter != T.finite_edges_end();
        iter++) {
      // edges are not represented as edges in CGAL triangulation graphs.
      // Instead, they are stored in faces/cells.

      Triangulation::Triangulation_data_structure::Edge e = *iter;
      Triangulation::Triangulation_data_structure::Cell_handle c = e.first;
      int i = e.second;
      int j = e.third;
//std::cout<<1448-i<<" "<<1448-j<<std::endl
      Triangulation::Triangulation_data_structure::Vertex_handle a = c->vertex(i);
      Point pa = a->point();
      Point pb = c->vertex(j)->point();
      if((Mapper.find(pa)->second==Mapper.find(pb)->second)/*||(Mapper.find(pa)->second==close_clusters[Mapper.find(pb)->second][0])||(Mapper.find(pa)->second==close_clusters[Mapper.find(pb)->second][1])||(Mapper.find(pa)->second==close_clusters[Mapper.find(pb)->second][2])||(Mapper.find(pa)->second==close_clusters[Mapper.find(pb)->second][3])||(Mapper.find(pa)->second==close_clusters[Mapper.find(pb)->second][4])||(Mapper.find(pb)->second==close_clusters[Mapper.find(pa)->second][0])||(Mapper.find(pb)->second==close_clusters[Mapper.find(pa)->second][1])||(Mapper.find(pb)->second==close_clusters[Mapper.find(pa)->second][2])||(Mapper.find(pb)->second==close_clusters[Mapper.find(pa)->second][3])||(Mapper.find(pb)->second==close_clusters[Mapper.find(pa)->second][4])*/){
      	std::cout<< pa<< std::endl<< pb<<std::endl;
      }
   }
   
    return 0;
}






