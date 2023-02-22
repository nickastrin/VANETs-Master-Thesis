import sched, time, os
import pandas as pd
import time
from pathlib import Path
from sklearn.cluster import KMeans

def export_data(scheduler): 
    start = time.time()
    print ("Doing stuff...")

    absolute_path = os.path.dirname(__file__)
    relative_path = "vehicles.csv"
    full_path = os.path.join(absolute_path, relative_path)

    file = Path(full_path)
    if file.exists():
        print("Calculating cluster centers")
        df = pd.read_csv(file)
        kmeans = KMeans(n_clusters = 1, random_state = 0, n_init = "auto").fit(df)

        print(kmeans.cluster_centers_)
        result = pd.DataFrame(kmeans.cluster_centers_)

        csv_name = "cluster_centers.csv"
        result_path = os.path.join(absolute_path, csv_name)
        result.to_csv(result_path, index = False, header = False)

        os.remove(file)

    else:
        print("File does not exist")
    
    end = time.time()
    print("Time elapsed: ", end - start)
    # do your stuff
    scheduler.enter(30, 1, export_data, (scheduler,))

myScheduler = sched.scheduler(time.time, time.sleep)
myScheduler.enter(0, 1, export_data, (myScheduler,))
myScheduler.run()