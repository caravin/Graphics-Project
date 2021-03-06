This folder contains preprocessing files for dataset formation

edges2dataset.py will take input file in "edges.txt" which should contain the traingle edges in format <x1 \n,x2 \n >( edge between point x1 and x2 where x1 and x2 sepreated by \n  )and list edges in described format ,that come from delauny traingulation , edges can be in any order
and print the dataset on the terminal in format
    <point ,{1,0,-1}>

    1 outside the object
    0 on the boundary
    -1 inside the object

traingl2dataset.py will take input file in "traingle.txt" which should contain the traingle in format <x1 \n  x2 \n x3 \n> where (points x1 x2 x3 froms a traingle) and list of traingle by describes format  that come from delauny traingulation , traingles  can be in any order

and print the dataset on the terminal in format

    <point ,{1,0,-1}>

    1 outside the object
    0 on the boundary
    -1 inside the object