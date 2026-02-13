import os
from pathlib import Path

def generate_source_tree(project_root, output_file, exclude_dirs=None, extensions=None):
    """
    生成源码目录树结构
    :param project_root: 项目根目录 (示例: "YourProject/Source")
    :param output_file: 输出文件路径 (示例: "SourceTree.txt")
    :param exclude_dirs: 需要排除的目录名集合 (默认排除虚幻引擎生成目录)
    :param extensions: 包含的文件扩展名集合 (默认: {'.h', '.cpp'})
    """
    # 参数默认值处理
    exclude_dirs = exclude_dirs or {'Intermediate', 
                                    'Build', 
                                    'DerivedDataCache', 
                                    'Saved', 
                                    'Binaries',
                                    '__ExternalObjects__',
                                    '__ExternalActors__',}
   # extensions = extensions or {'.h', '.cpp','.uasset','.umap'}
    extensions = extensions or {'.uasset','.umap'}
    project_root = Path(project_root).resolve()
    output_lines = [f"项目目录结构: {project_root.name}/"]

    def walk_dir(current_dir, indent_level, is_last):
        """递归遍历目录生成树形结构"""
        # 获取排序后的目录内容（先目录后文件）
        entries = sorted(os.listdir(current_dir), key=lambda x: (not os.path.isdir(os.path.join(current_dir, x)), x))
        
        for index, entry in enumerate(entries):
            entry_path = current_dir / entry
            relative_path = entry_path.relative_to(project_root)
            
            # 跳过排除目录
            if entry in exclude_dirs:
                continue
                
            # 判断是否是最后一个条目
            is_last_entry = index == len(entries) - 1
            
            # 生成连接符号
            connector = "└── " if is_last_entry else "├── "
            
            if entry_path.is_dir():
                # 处理目录
                output_lines.append(f"{indent_level}{connector}{entry}/")
                new_indent = indent_level + ("    " if is_last_entry else "│   ")
                walk_dir(entry_path, new_indent, is_last_entry)
            else:
                # 处理文件（仅包含指定扩展名）
                if entry_path.suffix in extensions:
                    output_lines.append(f"{indent_level}{connector}{entry}")

    try:
        walk_dir(project_root, "", False)
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(output_lines))
        print(f"成功生成目录树到: {output_file}")
    except Exception as e:
        print(f"生成失败: {str(e)}")

# 使用示例（修改路径后运行）
if __name__ == "__main__":
    generate_source_tree(
        project_root="G:\\UPS\\GitLab\\XGLyra\\Lyra_5.5\\Lyra\\Content",
        output_file="G:\\UPS\\GitLab\\XGLyra\\ProjectStructure.txt"
    )