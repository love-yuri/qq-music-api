<template>
  <div class="min-h-screen bg-linear-to-br from-blue-50 to-indigo-100 p-6 flex-col flex">
    <div class="bg-white rounded-2xl shadow-xl p-8 mb-6 w-full">
      <h1 class="text-4xl font-bold text-gray-800 mb-2">JSON转C++结构体工具</h1>
      <p class="text-gray-600">粘贴JSON,自动生成标准C++结构体定义</p>
    </div>

    <div class="flex flex-row flex-1 w-full">
      <!-- 输入区域 -->
      <div class="space-y-4 flex flex-col flex-1 mr-8">
        <div class="bg-white rounded-xl shadow-lg p-6">
          <label class="block text-sm font-semibold text-gray-700 mb-3">
            📋 粘贴JSON字符串
          </label>
          <textarea v-model="input" placeholder='{"name": "John", "age": 30, "address": {"city": "Beijing"}}'
            class="w-full h-48 bg-gray-50 text-gray-800 border-2 border-gray-200 rounded-lg p-4 font-mono text-sm focus:ring-2 focus:ring-blue-500 focus:border-blue-500 transition" />
        </div>

        <div v-if="error" class="bg-red-50 border-2 border-red-300 text-red-700 px-4 py-3 rounded-lg font-medium">
          ⚠️ {{ error }}
        </div>

        <div v-if="formatted" class="bg-white rounded-xl shadow-lg p-6 flex-1">
          <label class="block text-sm font-semibold text-gray-700 mb-3">
            ✨ 格式化后的JSON
          </label>
          <pre
            class="bg-green-50  text-green-800 border-2 border-green-200 rounded-lg p-4 overflow-auto text-sm">{{ formatted }}</pre>
        </div>
      </div>
      <!-- 字段选择和输出区域 -->
      <div class="space-y-4 w-[700px]">
        <template v-if="treeData.length > 0">
          <div class="bg-white rounded-xl shadow-lg p-6">
            <div class="flex items-center justify-between mb-3">
              <label class="text-sm font-semibold text-gray-700">
                🌳 字段树
              </label>
              <div class="flex gap-2">
                <button @click="expandAll"
                  class="text-xs bg-green-500 text-white px-3 py-1 rounded-md hover:bg-green-600 transition">
                  展开全部
                </button>
                <button @click="collapseAll"
                  class="text-xs bg-yellow-500 text-white px-3 py-1 rounded-md hover:bg-yellow-600 transition">
                  收起全部
                </button>
                <button @click="selectAll"
                  class="text-xs bg-blue-500 text-white px-3 py-1 rounded-md hover:bg-blue-600 transition">
                  全选
                </button>
                <button @click="deselectAll"
                  class="text-xs bg-gray-400 text-white px-3 py-1 rounded-md hover:bg-gray-500 transition">
                  全不选
                </button>
              </div>
            </div>
            <div class="bg-gray-50 border-2 border-gray-200 rounded-lg p-4 overflow-auto">
              <TreeNode v-for="node in treeData" :key="node.path" :node="node" :level="0" :selected="selected"
                :expanded="expanded" @toggle-expand="toggleExpand" @toggle-select="toggleSelect" />
            </div>
          </div>

          <div class="bg-white rounded-xl shadow-lg p-6">
            <div class="flex items-center gap-3 mb-3">
              <label class="text-sm font-semibold text-gray-700">
                🔧 主结构体名称
              </label>
              <input type="text" v-model="structName"
                class="flex-1 bg-gray-50 border-2 border-gray-200 rounded-lg px-3 py-1 text-sm focus:ring-2 focus:ring-blue-500 focus:border-blue-500" />
            </div>
            <div class="flex items-center justify-between mb-3">
              <label class="text-sm font-semibold text-gray-700">
                💻 生成的C++结构体
              </label>
              <button @click="copyToClipboard" :class="[
                'text-sm px-4 py-2 rounded-lg font-medium transition',
                copySuccess
                  ? 'bg-green-500 text-white'
                  : 'bg-blue-500 text-white hover:bg-blue-600'
              ]">
                {{ copySuccess ? '✓ 已复制!' : '📋 复制代码' }}
              </button>
            </div>
            <pre
              class="bg-linear-to-br from-purple-50 to-blue-50 text-gray-800 border-2 border-purple-200 rounded-lg p-4 overflow-auto max-h-96 text-sm font-mono whitespace-pre">{{ generateCppStruct() }}</pre>
          </div>
        </template>

        <div v-if="treeData.length === 0 && !error" class="bg-white rounded-xl shadow-lg p-12 text-center">
          <div class="text-6xl mb-4">📝</div>
          <p class="text-gray-500 text-lg">在左侧输入JSON字符串开始使用</p>
        </div>
      </div>
    </div>
  </div>
</template>



<script setup>
import { ref, watch, computed } from 'vue';
import { ChevronRight, ChevronDown } from 'lucide-vue-next';

const input = ref('');
const formatted = ref('');
const treeData = ref([]);
const selected = ref({});
const expanded = ref({});
const error = ref('');
const structName = ref('Root');
const copySuccess = ref(false);

const capitalize = (str) => {
  return str.charAt(0).toUpperCase() + str.slice(1);
};

const toPascalCase = (str) => {
  return str.split('_').map(word => capitalize(word)).join('');
};

const getCppType = (value, fieldName = '') => {
  if (value === null) return 'void*';
  if (typeof value === 'string') return 'std::string';
  if (typeof value === 'number') {
    return Number.isInteger(value) ? 'int' : 'double';
  }
  if (typeof value === 'boolean') return 'bool';
  if (Array.isArray(value)) {
    if (value.length === 0) return 'std::vector<void*>';
    if (typeof value[0] === 'object' && value[0] !== null && !Array.isArray(value[0])) {
      return `std::vector<${toPascalCase(fieldName)}Type>`;
    }
    return `std::vector<${getCppType(value[0])}>`;
  }
  if (typeof value === 'object') {
    return toPascalCase(fieldName) + 'Type';
  }
  return 'void*';
};

const buildTree = (obj, path = '') => {
  const nodes = [];

  for (const [key, value] of Object.entries(obj)) {
    const currentPath = path ? `${path}.${key}` : key;
    const isObject = typeof value === 'object' && value !== null && !Array.isArray(value);
    const isArrayOfObjects = Array.isArray(value) && value.length > 0 &&
      typeof value[0] === 'object' && value[0] !== null && !Array.isArray(value[0]);

    const node = {
      key: key,
      path: currentPath,
      type: getCppType(value, key),
      value: value,
      isObject: isObject,
      isArrayOfObjects: isArrayOfObjects,
      children: []
    };

    if (isObject) {
      node.children = buildTree(value, currentPath);
    } else if (isArrayOfObjects) {
      node.children = buildTree(value[0], currentPath);
    }

    nodes.push(node);
  }

  return nodes;
};

const collectAllPaths = (nodes, paths = []) => {
  nodes.forEach(node => {
    paths.push(node.path);
    if (node.children.length > 0) {
      collectAllPaths(node.children, paths);
    }
  });
  return paths;
};

const toggleExpand = (path) => {
  expanded.value = {
    ...expanded.value,
    [path]: !expanded.value[path]
  };
};

const toggleSelect = (node, checked) => {
  const pathsToUpdate = [node.path];
  if (node.children.length > 0) {
    pathsToUpdate.push(...collectAllPaths(node.children));
  }

  const newSelected = { ...selected.value };
  pathsToUpdate.forEach(path => {
    newSelected[path] = checked;
  });
  selected.value = newSelected;
};

const generateStructsFromTree = (nodes, parentPath = '') => {
  const structs = [];

  nodes.forEach(node => {
    if (node.isObject || node.isArrayOfObjects) {
      const structName = toPascalCase(node.key) + 'Type';
      const fields = [];

      node.children.forEach(child => {
        if (selected.value[child.path]) {
          fields.push({
            name: child.key,
            type: child.type
          });
        }
      });

      if (fields.length > 0) {
        structs.push({ name: structName, fields });
      }

      if (node.children.length > 0) {
        structs.push(...generateStructsFromTree(node.children, node.path));
      }
    }
  });

  return structs;
};

const generateCppStruct = () => {
  if (treeData.value.length === 0) return '// 请至少选择一个字段';

  let cpp = '#include <string>\n#include <vector>\n\n';

  const nestedStructs = generateStructsFromTree(treeData.value);
  const uniqueStructs = new Map();

  nestedStructs.forEach(struct => {
    if (!uniqueStructs.has(struct.name)) {
      uniqueStructs.set(struct.name, struct);
    } else {
      const existing = uniqueStructs.get(struct.name);
      struct.fields.forEach(field => {
        if (!existing.fields.find(f => f.name === field.name)) {
          existing.fields.push(field);
        }
      });
    }
  });

  const sortStructsByDependency = (structs) => {
    const structMap = new Map(structs);
    const sorted = [];
    const visited = new Set();
    const visiting = new Set();

    const visit = (name) => {
      if (visited.has(name)) return;
      if (visiting.has(name)) return;

      visiting.add(name);
      const struct = structMap.get(name);

      if (struct) {
        struct.fields.forEach(field => {
          const fieldType = field.type.replace(/std::vector<|>/g, '');
          if (structMap.has(fieldType) && fieldType !== name) {
            visit(fieldType);
          }
        });
      }

      visiting.delete(name);
      visited.add(name);
      if (struct) sorted.push(struct);
    };

    structMap.forEach((struct, name) => visit(name));
    return sorted;
  };

  const forwardDeclarations = Array.from(uniqueStructs.keys())
    .map(name => `struct ${name};`)
    .join('\n');

  cpp += forwardDeclarations + '\n\n';

  const sortedStructs = sortStructsByDependency(uniqueStructs);

  sortedStructs.forEach(struct => {
    cpp += `struct ${struct.name} {\n`;
    struct.fields.forEach(field => {
      cpp += `    ${field.type} ${field.name};\n`;
    });
    cpp += '};\n\n';
  });

  const rootFields = treeData.value.filter(node => selected.value[node.path]);

  if (rootFields.length > 0) {
    cpp += `struct ${structName.value} {\n`;
    rootFields.forEach(node => {
      cpp += `    ${node.type} ${node.key};\n`;
    });
    cpp += '};';
  } else {
    return '// 请至少选择一个字段';
  }

  return cpp;
};

watch(input, (newInput) => {
  if (!newInput.trim()) {
    formatted.value = '';
    treeData.value = [];
    selected.value = {};
    expanded.value = {};
    error.value = '';
    return;
  }

  try {
    const parsed = JSON.parse(newInput);
    formatted.value = JSON.stringify(parsed, null, 2);
    const tree = buildTree(parsed);
    treeData.value = tree;

    const allPaths = collectAllPaths(tree);
    const initialSelected = {};
    const initialExpanded = {};

    allPaths.forEach(path => {
      initialSelected[path] = true;
    });

    tree.forEach(node => {
      initialExpanded[node.path] = true;
    });

    selected.value = initialSelected;
    expanded.value = initialExpanded;
    error.value = '';
  } catch (e) {
    error.value = 'JSON格式错误: ' + e.message;
    formatted.value = '';
    treeData.value = [];
  }
});

const expandAll = () => {
  const allPaths = collectAllPaths(treeData.value);
  const newExpanded = {};
  allPaths.forEach(path => {
    newExpanded[path] = true;
  });
  expanded.value = newExpanded;
};

const collapseAll = () => {
  expanded.value = {};
};

const selectAll = () => {
  const allPaths = collectAllPaths(treeData.value);
  const newSelected = {};
  allPaths.forEach(path => {
    newSelected[path] = true;
  });
  selected.value = newSelected;
};

const deselectAll = () => {
  selected.value = {};
};

const copyToClipboard = async () => {
  const code = generateCppStruct();
  try {
    await navigator.clipboard.writeText(code);
    copySuccess.value = true;
    setTimeout(() => {
      copySuccess.value = false;
    }, 2000);
  } catch (err) {
    console.error('复制失败:', err);
  }
};
</script>

<script>
import { defineComponent, h, ref as vueRef, computed, watch } from 'vue';
import { ChevronRight, ChevronDown } from 'lucide-vue-next';

export const TreeNode = defineComponent({
  name: 'TreeNode',
  props: {
    node: Object,
    level: Number,
    selected: Object,
    expanded: Object
  },
  emits: ['toggle-expand', 'toggle-select'],
  setup(props, { emit }) {
    const checkboxRef = vueRef(null);

    const hasChildren = computed(() => props.node.children.length > 0);
    const isExpanded = computed(() => props.expanded[props.node.path]);

    const collectAllPaths = (nodes, paths = []) => {
      nodes.forEach(node => {
        paths.push(node.path);
        if (node.children.length > 0) {
          collectAllPaths(node.children, paths);
        }
      });
      return paths;
    };

    const isSelected = computed(() => {
      if (props.node.children.length === 0) {
        return props.selected[props.node.path] || false;
      }
      const allChildPaths = collectAllPaths(props.node.children);
      return allChildPaths.every(path => props.selected[path]);
    });

    const isIndeterminate = computed(() => {
      if (props.node.children.length === 0) return false;
      const allChildPaths = collectAllPaths(props.node.children);
      const selectedCount = allChildPaths.filter(path => props.selected[path]).length;
      return selectedCount > 0 && selectedCount < allChildPaths.length;
    });

    watch([isIndeterminate, isSelected], () => {
      if (checkboxRef.value) {
        checkboxRef.value.indeterminate = isIndeterminate.value;
      }
    });

    return () => h('div', [
      h('div', {
        class: 'flex items-center py-2 hover:bg-blue-50 rounded transition group',
        style: { paddingLeft: `${props.level * 20 + 8}px` }
      }, [
        hasChildren.value ? h('button', {
          onClick: () => emit('toggle-expand', props.node.path),
          class: 'mr-1 text-gray-500 hover:text-gray-700'
        }, [
          isExpanded.value ? h(ChevronDown, { size: 16 }) : h(ChevronRight, { size: 16 })
        ]) : h('span', { class: 'w-5 mr-1' }),

        h('input', {
          ref: checkboxRef,
          type: 'checkbox',
          checked: isSelected.value,
          onChange: (e) => emit('toggle-select', props.node, e.target.checked),
          class: 'w-4 h-4 text-blue-600 rounded focus:ring-2 focus:ring-blue-500'
        }),

        h('span', { class: 'ml-3 text-gray-800 font-mono text-sm flex-1' }, props.node.key),

        h('span', {
          class: 'text-blue-600 text-xs font-semibold bg-blue-100 px-2 py-1 rounded opacity-0 group-hover:opacity-100 transition'
        }, props.node.type)
      ]),

      hasChildren.value && isExpanded.value ? h('div',
        props.node.children.map(child =>
          h(TreeNode, {
            key: child.path,
            node: child,
            level: props.level + 1,
            selected: props.selected,
            expanded: props.expanded,
            onToggleExpand: (path) => emit('toggle-expand', path),
            onToggleSelect: (node, checked) => emit('toggle-select', node, checked)
          })
        )
      ) : null
    ]);
  }
});
</script>

<style scoped>
/* Tailwind CSS classes are used, no custom styles needed */
</style>