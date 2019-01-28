(function() {
var App = function App() {
  this.init();
};
App.prototype.init = function() {
  this.bindElements();
  this.bindEvents();
};
App.prototype.bindElements = function() {
  this.editor = document.getElementById('editor');
  this.cm = CodeMirror.fromTextArea(
      this.editor,
      {lineNumbers: true, gutters: ['CodeMirror-linenumbers', 'breakpoints']});
};
App.prototype.bindEvents = function() {
  window.addEventListener('haschange', function() {
    console.log(arguments);
  })
};
new App();
})();