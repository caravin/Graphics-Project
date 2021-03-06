import numpy as np
import random
import math

# class used while calculating ray intersection with traingle
class Vec3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def sub(self, v):
        return Vec3(self.x - v.x,
                    self.y - v.y,
                    self.z - v.z)

    def dot(self, v):
        return self.x * v.x + self.y * v.y + self.z * v.z

    def cross(self, v):
        return Vec3(self.y * v.z - self.z * v.y,
                    self.z * v.x - self.x * v.z,
                    self.x * v.y - self.y * v.x)

    def length(self):
        return math.sqrt(self.x * self.x +
                         self.y * self.y +
                         self.z * self.z)

    def normalize(self):
        l = self.length()
        return Vec3(self.x / l, self.y / l, self.z / l)


class Ray:
    def __init__(self, orig=None, direction=None):
        self.orig = orig
        self.direction = direction

# return '-inf' if ray r doesn't intersect the triangle formed by v0,v1,v2 else return distance of rays origin from the traingle with sign 
def ray_triangle_intersect(r, v0, v1, v2):
    v0v1 = v1.sub(v0)
    v0v2 = v2.sub(v0)
    pvec = r.direction.cross(v0v2)

    det = v0v1.dot(pvec)

    if det < 0.000001:
        return float('-inf')

    invDet = 1.0 / det
    tvec = r.orig.sub(v0)
    u = tvec.dot(pvec) * invDet

    if u < 0 or u > 1:
        return float('-inf')

    qvec = tvec.cross(v0v1)
    v = r.direction.dot(qvec) * invDet

    if v < 0 or u + v > 1:
        return float('-inf')

    return v0v2.dot(qvec) * invDet

#  add traingle in traingle_set
def add_traingle(A,B,C,traingle_set):
    if((A,B,C) in traingle_set):
        return
    if((A,C,B) in traingle_set):
        return
    if((B,A,C) in traingle_set):
        return
    if((B,C,A) in traingle_set):
        return
    if((C,A,B) in traingle_set):
        return
    if((C,B,A) in traingle_set):
        return
    traingle_set.add((A,B,C))

f=open("edges.txt","r")
f1=f.readlines()
f.close()

points = list()
for s in f1:
    s = s[:-1]
    s=s.split()
    s[0] = float(s[0])
    s[1] = float(s[1])
    s[2] = float(s[2])
    points.append(tuple(s))

vertices = set()

for point in points: 
    vertices.add(point)

edges = set()
for i in range(0,len(points),2):
    edges.add((points[i],points[i+1]))

traingle_set = set()

for edge in edges:
    for vertice in vertices:
        if(vertice == edge[0] or vertice == edge[1] ):
            continue
        if( ((vertice,edge[0]) in edges or (edge[0],vertice) in edges) and ((vertice,edge[1]) in edges or (edge[1],vertice) in edges) ):
            add_traingle(vertice,edge[0],edge[1],traingle_set)

traingles = list()

for key in traingle_set:
    traingles.append(key)

print(len(traingles))

#  data -> point , D -> direction  ,
# returns count % 2 where count is no trinagle intersect with ray (data--->D) and if point lies on the one of traingle return 0;
def cal(data,D):
    O = np.array([data[0],data[1],data[2]])
    count = 0
    h = 1
    for traingle in traingles:
        print(h)
        A = np.array([traingle[0][0], traingle[0][1], traingle[0][2]])
        B = np.array([traingle[1][0], traingle[1][1], traingle[1][2]])
        C = np.array([traingle[2][0], traingle[2][1], traingle[2][2]])
        ret = find(O,D,A,B,C)
        if ret == -1:
            count = -1
        else:
            count += ret
        if count < 0 :
            break
        h = h + 1
    if count >= 0:
        return count%2
    else:
        return 2

print_count = 0

#  dataset formatation 
data_set = list()
for i in range(0,100):  # no of iteration for each traingle 
    for traingle in traingles:
        A = np.array([traingle[0][0], traingle[0][1], traingle[0][2]])
        B = np.array([traingle[1][0], traingle[1][1], traingle[1][2]])
        C = np.array([traingle[2][0], traingle[2][1], traingle[2][2]])

        N = np.cross(B-A,C-A)
        N = N / math.sqrt(np.square(N).sum())
        x = random.uniform(0, 1)
        y = random.uniform(0, 1)
        vx = (C-A) * x + A
        vy = (B-A) * y + A
        vz = (vx+vy)/2

        #  no of point in dataset for each traingle in one iteration
        TN = np.array([1,0,0])
        data_set.append((vz[0],vz[1],vz[2]))
        new_point = vz + N*0.25     # 0.25 is the precision , u can change it as precision u want
        data_set.append((new_point[0],new_point[1],new_point[2]))
        new_point = vz - N*0.25     # 0.25 is the precision , u can change it as precision u want
        data_set.append((new_point[0],new_point[1],new_point[2]))

print(len(traingles))

#  dataset contains data points, this loop print the point location wrt to object
for data in data_set:
    val = [0,0,0]
    cur_val = cal(data,np.array([1,0,0]))
    val[cur_val] = val[cur_val] + 1

    cur_val = cal(data,np.array([0,1,0]))
    val[cur_val] = val[cur_val] + 1

    cur_val = cal(data,np.array([1,0,0]))
    val[cur_val] = val[cur_val] + 1
    
    # 5 = (no of random directions for each dataset to find postion wrt datapoint)
    for ran_d in np.random.rand(5,3):
        cur_val = cal(data,ran_d)
        val[cur_val] = val[cur_val] + 1

    if(val[2] >= val[1] and val[2] >= val[0]):
        print("{0}  {1}   {2}     0".format(data[0],data[1],data[2]))
    elif(val[1] >= val[2] and val[1] >= val[0]):
        print("{0}  {1}   {2}    -1").format(data[0],data[1],data[2])
    else:
        print("{0}  {1}   {2}     1").format(data[0],data[1],data[2])
    print_count = print_count + 1
