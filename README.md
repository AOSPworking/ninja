# ninja-hack

## 1. Usage

```shell
> python configure.py --bootstrap
```

```shell
> ./ninja -f new-build.ninja -t origin frameworks/opt/telephony/src/java/android/telephony/gsm/SmsManager.java > SmsManager.json
```

## 2. Description

关于这个 `json` 文件，里面结构是这样的：

```json
{
    "nodes": [
        {
            "name": str,
            "distance": int
        }
    ],
    "edges": [
        {
            "target": [str],
            "source": [str],
            "impact_source": [str]
        }
    ],
    "node_num": int,
    "edge_nun": int
}
```

可以输出影响的每一个边和经过的节点，而且是从最底层的开始输出。

由此可以得到对 “‘每一个 `impact_source` 中属于源代码’ 的文件所在模块” 的影响。
