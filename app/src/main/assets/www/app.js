(function() {
function replaceLine(cm, text, lineNumber, lineLength) {
  cm.replaceRange(
      text, {line: lineNumber, ch: 0}, {line: lineNumber, ch: lineLength});
}
// string callback(string)
function replaceInLine(cm, callback) {
  var start = cm.getCursor('start');
  var end = cm.getCursor('end');
  // prevent change multiline
  if (start.line != end.line) return;
  var select = cm.getLine(start.line).substr(start.ch, end.ch - start.ch);

  var text = callback(select);
  cm.replaceRange(text, start, end);
}
function list(cm) {
  var {start, end} = Formatter.position(cm);
  var len = 0;
  for (var i = start.line; i < end.line + 1; i++) {
    var text = cm.getLine(i);
    len = text.length;

    if (text.charAt(0) == '*' && text.charAt(1) == ' ') {
      text = text.substring(2);
      console.log(text);
    } else {
      text = '* ' + text;
    }
    replaceLine(cm, text, i, len);
  }
  cm.setSelection({line: start.line, ch: 0}, {line: end.line, ch: len})
}
var Formatter = {
  list: list,
  position: function(cm) {
    return {
      start: cm.getCursor('start'), end: cm.getCursor('end')
    }
  },
  heading: function(cm) {
    var {start, end} = Formatter.position(cm);
    var len = 0;
    for (var i = start.line; i < end.line + 1; i++) {
      var text = cm.getLine(i);
      if (text.charAt(0) == '#') {
        text = '#' + text;
      } else {
        text = '# ' + text;
      }
      len = text.length;
      cm.replaceRange(text, {line: i, ch: 0}, {line: i, ch: len});
    }
    cm.setSelection({line: start.line, ch: 0}, {line: end.line, ch: len})
  },
  bold: function(cm) {
    replaceInLine(cm, function(selected) {
      return ` **${selected}** `;
    })
  },
  link: function(cm) {
    replaceInLine(cm, function(selected) {
      return ` [${selected}]() `;
    })
  },
  code: function(cm) {
    var {start, end} = Formatter.position(cm);
    if (start.line != end.line) {
      var str = [];
      for (var i = start.line; i < end.line + 1; i++) {
        str.push(cm.getLine(i));
      }
      cm.replaceRange(
          `\`\`\`\n${str.join('\n')}\n\`\`\`\n`, {line: start.line, ch: 0},
          {line: end.line, ch: cm.getLine(end.line).length})
    } else {
      var select = cm.getLine(start.line).substr(start.ch, end.ch - start.ch);

      var text = ` \`${select}\` `;
      cm.replaceRange(text, start, end);
    }
  }
};

function onClick(id, callback) {
  var element = document.getElementById(id);
  if (element)
    element.addEventListener('click', callback);
  else
    console.error(`cant find the elemnt: id = ${id} \n`);
}
var App = function App() {
  this.init();
};
App.prototype.init = function() {
  this.bindElements();
  this.bindEvents();
  this.setupElements();
};

App.prototype.onUpdate = function() {
  var obj = this.collect();
  if (!obj) return;
  fetch('/api/update', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json; charset=utf-8',
    },
    body: JSON.stringify(obj)
  })
      .then(response => {
        if (response.status == 404) throw new Error('Not Found');
        return response.json();
      })
      .then(data => {
        this.title.innerText = obj['title'];
        document.title = obj['title'];
        Toast.onShow('成功', '成功更新文档');
        if (data['id']) window.location.hash = data['id'];
      })
      .catch((e) => {
        console.error(e);
        Toast.onShow('错误', e + ' ');
      });
};
App.prototype.collect = function() {
  var obj = {};
  var hash = window.location.hash;
  if (hash.length > 0) hash = hash.substring(1);
  obj['id'] = parseInt(hash) || -1;
  var value = this.cm.getValue().trim();
  if (value.length < 1) return null;
  ;
  var p = value.indexOf('\n');
  var title = '';
  if (p != -1) {
    title = value.substring(0, p).trim();
  } else {
    title = value;
  }
  obj['title'] = title;
  obj['content'] = value;
  return obj;
};

App.prototype.bindElements = function() {
  this.title = document.getElementById('title');
  this.editor = document.getElementById('editor');
  this.article = document.querySelector('.article');
  this.cm = CodeMirror.fromTextArea(this.editor, {
    lineNumbers: true,
    lineWrapping: true,
    gutters: ['CodeMirror-linenumbers', 'breakpoints']
  });
  this.CodeMirror = document.querySelector('.CodeMirror');
};
App.prototype.setupElements = function() {
  if (this.cm) {
    this.cm.addKeyMap({
      'F1': this.onUpdate.bind(this),
      'F2': this.code,
      'Ctrl-H': this.heading,
      'Ctrl-P': this.preview,
      'Esc': () => {
        window.location.hash = '';
        this.cm.setValue('');
        this.title.innerText = '新文档';
      }
    });
  } else {
    console.error('faied: setupElements');
  }
};
App.prototype.onHashChange = function(e) {
  var hash = window.location.hash;
  this.onFetch(hash.substring(1));
};
App.prototype.onFetch = function(hash) {
  fetch('/api/get/' + hash, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json; charset=utf-8',
    }
  })
      .then(response => response.json())
      .then(data => {
        let value = data['content'];
        this.title.innerText = data['title'];
        this.cm.setValue(value);
      });
};
App.prototype.bindEvents = function() {
  window.addEventListener('hashchange', this.onHashChange.bind(this));
  this.heading = function() {
    Formatter.heading(this.cm);
  }.bind(this);
  this.bold = function() {
    Formatter.bold(this.cm);
  }.bind(this);
  this.code = function() {
    Formatter.code(this.cm);
  }.bind(this);
  this.preview = function() {
    if (this.CodeMirror.classList.contains('hide')) {
      this.article.innerHTML = '';
      this.CodeMirror.classList.remove('hide');
    } else {
      this.article.innerHTML = marked(this.cm.getValue());
      this.CodeMirror.classList.add('hide');
    }
  }.bind(this);
  this.list = function() {
    Formatter.list(this.cm);
  }.bind(this);
  this.link=function(){
    Formatter.link(this.cm);
  }.bind(this);
  onClick('link',this.link);
  onClick('list', this.list);
  onClick('heading', this.heading);
  onClick('bold', this.bold);
  onClick('preview', this.preview);
};
new App();
})();