(function() {
var Formatter = {
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
    var {start, end} = Formatter.position(cm);
    if (start.line != end.line) return;
    
    var text = `**${cm.getLine(i)}**`;
    cm.replaceRange(text, start, end);
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
  this.setupElements();
  this.bindEvents();
};
App.prototype.bindElements = function() {
  this.editor = document.getElementById('editor');

  this.cm = CodeMirror.fromTextArea(
      this.editor,
      {lineNumbers: true, gutters: ['CodeMirror-linenumbers', 'breakpoints']});
};
App.prototype.setupElements = function() {
  if (this.cm) {
    this.cm.addKeyMap({'Ctrl-H': this.heading});
  }
};

App.prototype.bindEvents = function() {
  window.addEventListener('haschange', function() {
    console.log(arguments);
  });
  this.heading = function() {
    Formatter.heading(this.cm);
  }.bind(this);
  this.bold = function() {
    Formatter.bold(this.cm);
  }.bind(this);
  onClick('heading', this.heading);
  onClick('bold', this.bold);
};
new App();
})();