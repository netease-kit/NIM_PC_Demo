((obj) => {
  const g = {}

  g.showMsgBox = (content, title = "提示", confirm = "确定", cancel = "取消") => {
    obj.call('ShowMsgBox', { title, content, confirm, cancel }, (error, result) => {
      if (error) {
        console.log(result)
      }
    })
  }

  obj.profile = g
})(NimCefWebInstance)