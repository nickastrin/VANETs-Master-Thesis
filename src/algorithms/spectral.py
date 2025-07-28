import sched, time, os
import pandas as pd
import time
from pathlib import Path
from sklearn.cluster import KMeans
from sklearn.cluster import SpectralClustering

def export_data(): 
    start = time.time()
    print ("Doing stuff...")

    absolute_path = os.path.dirname(__file__)
    relative_path = "vehicles.csv"
    full_path = os.path.join(absolute_path, relative_path)

    file = Path(full_path)
    if file.exists():
        df = pd.read_csv(file)
        clustering = SpectralClustering(n_clusters=3, 
                assign_labels='discretize', random_state=0).fit(df)
        
        cluster_one_x_centroid = 0
        cluster_one_y_centroid = 0
        size_one = 0
        cluster_two_x_centroid = 0
        cluster_two_y_centroid = 0
        size_two = 0
        cluster_three_x_centroid = 0
        cluster_three_y_centroid = 0
        size_three = 0
        for i in range(0, len(clustering.labels_)):
            if clustering.labels_[i] == 0:
                cluster_one_x_centroid += df.iloc[i][0]
                cluster_one_y_centroid += df.iloc[i][1]
                size_one += 1
            elif clustering.labels_[i] == 1:
                cluster_two_x_centroid += df.iloc[i][0]
                cluster_two_y_centroid += df.iloc[i][1]
                size_two += 1
            else:
                cluster_three_x_centroid += df.iloc[i][0]
                cluster_three_y_centroid += df.iloc[i][1]
                size_three += 1

        cluster_one_x_centroid = cluster_one_x_centroid / size_one
        cluster_one_y_centroid = cluster_one_y_centroid / size_one
        cluster_two_x_centroid = cluster_two_x_centroid / size_two
        cluster_two_y_centroid = cluster_two_y_centroid / size_two
        cluster_three_x_centroid = cluster_three_x_centroid / size_three
        cluster_three_y_centroid = cluster_three_y_centroid / size_three

        result = pd.DataFrame([[cluster_one_x_centroid, cluster_one_y_centroid], [cluster_two_x_centroid, cluster_two_y_centroid], [cluster_three_x_centroid, cluster_three_y_centroid]])

        csv_name = "cluster_centers.csv"
        result_path = os.path.join(absolute_path, csv_name)
        result.to_csv(result_path, index = False, header = False)

        os.remove(file)
    else:
        print("File does not exist")
    
    end = time.time()
    print("Time elapsed: ", end - start)
    # do your stuff

myScheduler = sched.scheduler(time.time, time.sleep)
myScheduler.enter(0, 1, export_data, (myScheduler,))
myScheduler.run()