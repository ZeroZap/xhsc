#!/bin/bash

# 使用方法: ./rename_files.sh <directory> <file_extension> <prefix_or_suffix> <string_or_number> [recursive]
# 例如:
# - 在文件名前面添加字符串 "example_": ./rename_files.sh ./ txt prefix "example_"
# - 在文件名后面添加顺序数字: ./rename_files.sh ./ txt suffix number
# - 递归处理: ./rename_files.sh ./ txt prefix "example_" true/falsh

# 检查参数是否正确
if [ $# -lt 4 ] || [ $# -gt 5 ]; then
    echo "Usage: $0 <directory> <file_extension> <prefix_or_suffix> <string_or_number> [recursive]"
    echo "Example: $0 ./ txt prefix \"example_\""
    echo "Example: $0 ./ txt suffix number recursive"
    exit 1
fi

# 参数
DIRECTORY=$1
EXTENSION=$2
POSITION=$3 # prefix 或 suffix
STRING_OR_NUMBER=$4
RECURSIVE=${5:-false} # 默认为非递归

# 检查是否为有效的目录
if [ ! -d "$DIRECTORY" ]; then
    echo "Error: Directory $DIRECTORY does not exist."
    exit 1
fi

# 检查 prefix_or_suffix 是否有效
if [[ "$POSITION" != "prefix" && "$POSITION" != "suffix" ]]; then
    echo "Error: The third parameter must be 'prefix' or 'suffix'."
    exit 1
fi

# 初始化计数器（仅当是数字序列时）
COUNTER=1

# 根据是否递归处理文件
if [[ "$RECURSIVE" == "true" ]]; then
    FIND_CMD="find \"$DIRECTORY\" -type f -name \"*.$EXTENSION\""
else
    FIND_CMD="find \"$DIRECTORY\" -maxdepth 1 -type f -name \"*.$EXTENSION\""
fi

# 遍历文件
eval $FIND_CMD | while read -r FILE; do
    # 获取文件的父目录和文件名
    DIR=$(dirname "$FILE")
    BASENAME=$(basename "$FILE" ".$EXTENSION")

    # 根据用户的选择构造新的文件名
    if [ "$STRING_OR_NUMBER" == "number" ]; then
        if [ "$POSITION" == "prefix" ]; then
            NEW_NAME="${COUNTER}_${BASENAME}.${EXTENSION}"
        else
            NEW_NAME="${BASENAME}_${COUNTER}.${EXTENSION}"
        fi
        COUNTER=$((COUNTER + 1))
    else
        if [ "$POSITION" == "prefix" ]; then
            NEW_NAME="${STRING_OR_NUMBER}${BASENAME}.${EXTENSION}"
        else
            NEW_NAME="${BASENAME}${STRING_OR_NUMBER}.${EXTENSION}"
        fi
    fi

    # 重命名文件
    mv -v "$FILE" "${DIR}/${NEW_NAME}"
done

echo "Renaming complete!"