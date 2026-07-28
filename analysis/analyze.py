# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Vodolazskaya Elizaveta

import os
import glob
import pandas as pd
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt


ITERATIONS = {
    "web-Stanford": 200,
    "web-Google": 200,
    "hollywood-2009": 50,
    "mycielskian17": 50,
    "indochina-2004": 35,
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
            'Graph': graph,
            'Implementation': impl,
            'Iterations': iters,
            'Mean Time (seconds)': round(analysis['mean'], 6),
            'Std Dev (seconds)': round(analysis['std'], 6),
            'RSD (%)': round(analysis['rel_std_pct'], 2),
            '95% CI (seconds)': round(analysis['ci'], 6)
        })
    
    df = pd.DataFrame(table_data)
    df.to_csv(output_file, index=False)
    
    return df

def plot_performance_comparison(df, title="Performance Comparison"):
    
    graphs = df['Graph'].unique()
    lagraph_times = df[df['Implementation'] == 'lagraph']['Mean Time (seconds)'].values
    my_times = df[df['Implementation'] == 'my']['Mean Time (seconds)'].values
    
    x = np.arange(len(graphs))
    width = 0.35
    
    fig, ax = plt.subplots(figsize=(12, 7))
    
    
    bars1 = ax.bar(x - width/2, lagraph_times, width, label='lagraph', 
                   color='#2E86AB', edgecolor='black', linewidth=0.5)
    bars2 = ax.bar(x + width/2, my_times, width, label='my', 
                   color='#E84855', edgecolor='black', linewidth=0.5)
    
    for bar in bars1:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                f'{height:.2f}s', ha='center', va='bottom', fontsize=9)
    
    for bar in bars2:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                f'{height:.2f}s', ha='center', va='bottom', fontsize=9)
    
    
    for i, (l_time, m_time) in enumerate(zip(lagraph_times, my_times)):
        ratio = m_time / l_time
        ax.text(i, max(l_time, m_time) + 0.3, 
                f'{ratio:.1f}x', ha='center', va='bottom', 
                fontsize=10, fontweight='bold', 
                color='green' if ratio < 1 else 'red')
    
    ax.set_xlabel('Graph', fontsize=12, fontweight='bold')
    ax.set_ylabel('Mean Time (seconds)', fontsize=12, fontweight='bold')
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(graphs, rotation=45, ha='right')
    ax.legend(fontsize=11, loc='upper left')
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    ax.set_axisbelow(True)
    
    plt.tight_layout()
    return fig, ax

def plot_stability_dashboard(df, title="Measurement Stability"):

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
    
    
    df['CI_percent'] = df['95% CI (seconds)'] / df['Mean Time (seconds)'] * 100
    df_pivot_pct = df.pivot(index='Graph', columns='Implementation', values='CI_percent')
    df_pivot_pct.plot(kind='bar', ax=ax1, color=['#2E86AB', '#E84855'])
    ax1.set_title('95% CI (% of mean)', fontsize=11, fontweight='bold')
    ax1.set_ylabel('CI (%)')
    ax1.legend(fontsize=9)
    ax1.grid(axis='y', alpha=0.3)
    ax1.axhline(y=5, color='red', linestyle='--', alpha=0.5, label='5% threshold')
    
   
   
    df_pivot_rsd = df.pivot(index='Graph', columns='Implementation', values='RSD (%)')
    df_pivot_rsd.plot(kind='bar', ax=ax2, color=['#2E86AB', '#E84855'])
    ax2.set_title('Relative Standard Deviation', fontsize=11, fontweight='bold')
    ax2.set_ylabel('RSD (%)')
    ax2.legend(fontsize=9)
    ax2.grid(axis='y', alpha=0.3)
    ax2.axhline(y=5, color='red', linestyle='--', alpha=0.5, label='5% threshold')
    
    fig.suptitle(title, fontsize=14, fontweight='bold')
    plt.tight_layout()
    return fig, (ax1, ax2)

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
    df = write_csv(results, csv_file)

    print(f"CSV saved to: {csv_file}")

    fig1, ax1 = plot_performance_comparison(df)
    fig1.savefig(os.path.join(script_dir, "performance_comparison.png"), dpi=300, bbox_inches='tight')
    plt.close(fig1)
    print(f"  - Saved: performance_comparison.png")

    fig2, ax2 = plot_stability_dashboard(df)
    fig2.savefig(os.path.join(script_dir, "stability.png"), dpi=300, bbox_inches='tight')
    plt.close(fig2)
    print(f"  - Saved: stability_ci.png")

if __name__ == "__main__":
    main()