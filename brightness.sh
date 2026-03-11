#!/bin/bash
# 用法: ./brightness.sh <亮度值 0.0~1.0>
# 示例: ./brightness.sh 0.05   # 极暗
#       ./brightness.sh 1.0     # 恢复正常

# 检查参数数量
if [ $# -ne 1 ]; then
    echo "❌ 错误：需要指定一个亮度值（0.0 到 1.0）"
    echo "用法: $0 <亮度值>"
    exit 1
fi

brightness=$1

# 验证亮度值是否为有效数字且在 0~1 范围内
if ! [[ "$brightness" =~ ^[0-9]*\.?[0-9]+$ ]]; then
    echo "❌ 错误：亮度值必须为数字（例如 0.05, 1.0）"
    exit 1
fi

if (( $(echo "$brightness < 0 || $brightness > 1" | bc -l) )); then
    echo "❌ 错误：亮度值必须在 0.0 到 1.0 之间"
    exit 1
fi

# 定义你的两个显示器
displays=("HDMI-0" "DP-0")

# 逐个应用亮度设置
for display in "${displays[@]}"; do
    echo "🖥️  设置 $display 亮度为 $brightness"
    xrandr --output "$display" --brightness "$brightness"
done

echo "✅ 亮度设置完成"
