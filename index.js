const express = require("express");

const port = 3000;
const app = express();

app.set("views", `${__dirname}/views`);
app.set("view engine", "pug");

app.get("/", (req, res) => res.render("index.pug"));

app.get("/page", (req, res) => {
  const { page } = req.query;
  res.render("board.pug", { page });
});

app.get("/posts", (req, res) => {
  const { until } = req.query;
  const untilParsed = parseInt(until, 10);
  const posts = [];
  if (!isNaN(untilParsed)) {
    for (let i = 0; i < untilParsed; i++) {
      posts.push(`Post ${i + 1}`);
    }
  }
  res.render("posts.pug", { posts });
});

app.listen(port, () => console.log(`Server listening on port ${port}!`));
