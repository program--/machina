#' Create an AST from R code
#' See: https://stackoverflow.com/a/60083792
#' @param expr an R expression
#' @export
get_ast <- function(expr) purrr::map_if(as.list(expr), is.call, get_ast)

#' Convert R expression string to AST
#' @export
str2ast <- function(str) get_ast(str2expression(str))

#' Parse R file to an AST
#' @export
file2ast <- function(filename) str2ast(readLines(filename))
