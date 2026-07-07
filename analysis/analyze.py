import os
import glob
import pandas as pd
import numpy as np
from scipy import stats

ITERATIONS = {
    "web-Stanford": 200,
    "web-Google": 100,
    "hollywood-2009": 30,
    "indochina-2004": 15,
}

def get_results_dir():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(script_dir, "..", "benchmarks", "results")

def load_measurements(csv_file):
    if not os.path.exists(csv_file):
        return None
    df = pd.read_csv(csv_file, header=None, names=['time'])
    return df['time'].values

def collect_results(results_dir):
    results = []
    for csv_file in glob.glob(f"{results_dir}/**/*.csv", recursive=True):
        parts = csv_file.split(os.sep)
        if len(parts) >= 2:
            graph = parts[-2]
            impl = parts[-1].replace('.csv', '')
        times = load_measurements(csv_file)
        if times is not None and len(times) > 0:
            results.append({
                'graph': graph,
                'impl': impl,
                'times': times,
            })
    return results

def analyze_measurements(times):
    n = len(times)
    if n == 0:
        return None
    
    mean = np.mean(times)
    std = np.std(times, ddof=1)
    rel_std = std / mean * 100 if mean > 0 else float('inf')
    
    if n > 1:
        sem = stats.sem(times)
        t_critical = stats.t.ppf(0.975, df=n-1)
        ci = t_critical * sem
    else:
        ci = 0
    
    return {
        'mean': mean,
        'std': std,
        'rel_std_pct': rel_std,
        'ci': ci
    }

def format_ci(mean, ci):
    return f"{mean:.6f} ± {ci:.6f}"


def write_csv(results, output_file):
    table_data = []
    for r in results:
        analysis = r['analysis']
        graph = r['graph']
        impl = r['impl'].replace('_pagerank', '')
        iters = ITERATIONS.get(graph, "N/A")
        
        table_data.append({
            'graph': graph,
            'implementation': impl,
            'iterations': iters,
            'mean_time': round(analysis['mean'], 6),
            'std_dev': round(analysis['std'], 6),
            'rsd_percent': round(analysis['rel_std_pct'], 2),
            'ci_95': round(analysis['ci'], 6)
        })
    
    df = pd.DataFrame(table_data)
    df.to_csv(output_file, index=False)

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    results_dir = get_results_dir()
    
    if not os.path.exists(results_dir):
        print(f"Results directory not found: {results_dir}")
        return
    
    results = collect_results(results_dir)
    
    if not results:
        print(f"No measurement files found in {results_dir}")
        return
    
    for r in results:
        r['analysis'] = analyze_measurements(r['times'])
    

    csv_file = os.path.join(script_dir, "summary.csv")
    
    write_csv(results, csv_file)
    

    print(f"CSV saved to: {csv_file}")

if __name__ == "__main__":
    main()